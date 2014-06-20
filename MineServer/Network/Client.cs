using System;
using System.Collections.Generic;
using System.Net.Sockets;

namespace MineServer
{
	public class Client
	{
		private Socket workSocket;
		public Socket WorkSocket
		{
			get { return workSocket; }
		}
		private const int readCapacity = 1024;

		private BytesBuffer bufRead;
		private byte[] tmpBufRead;

		private Protocol _protocol;
		private Logger _logger;
		private Server _serv;
		private Player _player;
		private bool _destroyed = false;
		private uint _clientID;
		public uint ClientID { get { return _clientID; } }

		private static uint _NEXTID = 0;

		public Protocol protocol { get { return _protocol; } }

		public Client(Socket sock, Server serv)
		{
			workSocket = sock;
			_protocol = new Protocol(this, serv.world);
			_serv = serv;
			_logger = Logger.Instance;
			_clientID = _NEXTID++;
			bufRead = new BytesBuffer();
			tmpBufRead = new byte[readCapacity];
			workSocket.BeginReceive(tmpBufRead, 0, readCapacity, 0,
									new AsyncCallback(ReadCallback), this);
		}

		public void SetPlayer(Player player)
		{
			if (_player == null)
				_player = player;
		}

		private void ReadCallback(IAsyncResult ar)
		{
			Client client = (Client)ar.AsyncState;
			Socket handler = client.workSocket;
			int bytesRead;
			try
			{
				bytesRead = handler.EndReceive(ar);
			}
			catch (Exception ex)
			{
				if (ex is SocketException || ex is ObjectDisposedException)
				{
					System.Diagnostics.Debug.WriteLine(ex.ToString());
					Destroy();
					return;
				}
				throw;
			}
			if (bytesRead > 0)
			{
				client.bufRead.Write(client.tmpBufRead, 0, bytesRead);
				long pos = client.bufRead.Position;
				int size = _protocol.HandlePacket(client.bufRead);
				client.bufRead.Position = pos;
				if (size > 0)
				{
					BytesBuffer tmp = new BytesBuffer();
					tmp.Write(client.bufRead.GetBuffer(), size, (int)client.bufRead.Length - size);
					client.bufRead = tmp;
				}
				try
				{
					handler.BeginReceive(tmpBufRead, 0, readCapacity, 0,
										 new AsyncCallback(ReadCallback), this);
				}
				catch (Exception ex)
				{
					if (ex is SocketException || ex is ObjectDisposedException)
					{
						System.Diagnostics.Debug.WriteLine(ex.ToString());
						Destroy();
						return;
					}
					throw;
				}
			}
			else
				Destroy();
		}

		public void Send(byte[] buffer, int offset, int size)
		{
			try
			{
				workSocket.BeginSend(buffer, offset, (int)size, 0,
									 new AsyncCallback(SendCallback), workSocket);
			}
			catch (Exception ex)
			{
				if (ex is SocketException || ex is ObjectDisposedException)
				{
					System.Diagnostics.Debug.WriteLine(ex.ToString());
					Destroy();
					return;
				}
				throw;
			}
		}

		private void SendCallback(IAsyncResult ar)
		{
			try
			{
				Socket handler = (Socket)ar.AsyncState;
				int bytesSent = handler.EndSend(ar);
			}
			catch (Exception ex)
			{
				if (ex is SocketException || ex is ObjectDisposedException)
				{
					System.Diagnostics.Debug.WriteLine(ex.ToString());
					Destroy();
					return;
				}
				throw;
			}
		}

		public void Destroy()
		{
			lock (this)
			{
				if (_destroyed == true)
					return;
				if (_player != null)
					_serv.world.RemovePlayer(_player);
				_serv.DestroyClient(this);
				_destroyed = true;
			}
		}
	}
}
