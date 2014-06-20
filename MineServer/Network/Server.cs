using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace MineServer
{
	public class Server
	{
		public class IsRunning
		{
			public bool Running;
		}
		private IsRunning _running;
		private Logger _logger;
		public World world { get; set; }
		private ManualResetEvent allDone = new ManualResetEvent(false);
		private List<Client> clientList;
		public List<Client> ClientList { get { return clientList; } }
		private int _port;

		public Server(int port = 1337)
		{
			_running = new IsRunning();
			_running.Running = false;
			_port = port;
			_logger = Logger.Instance;
			clientList = new List<Client>();
		}

		public void Stop()
		{
			lock (clientList)
			{
				while (clientList.Count != 0)
					clientList[0].Destroy();
			}
			lock (_running)
			{
				_running.Running = false;
			}
			allDone.Set();
		}

		public void Run()
		{
			IPHostEntry ipHostInfo = Dns.Resolve(Dns.GetHostName());
			//IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
			IPAddress ipAddress = ipHostInfo.AddressList[0];
			IPEndPoint localEndPoint = new IPEndPoint(ipAddress, _port);
			Socket listener = new Socket(AddressFamily.InterNetwork,
										 SocketType.Stream, ProtocolType.Tcp);
			_logger.Log("Server started on " + ipAddress.ToString() + ":" + _port);
			try
			{
				listener.Bind(localEndPoint);
				listener.Listen(100);
				_running.Running = true;

				while (true)
				{
					allDone.Reset();
					_logger.Log("Waiting for a connection...");
					listener.BeginAccept(new AsyncCallback(AcceptCallback), listener);
					allDone.WaitOne();
					lock (_running)
					{
						if (_running.Running == false)
							break;
					}
				}
			}
			catch (Exception ex)
			{
				if (ex is SocketException || ex is ObjectDisposedException)
				{
					System.Diagnostics.Debug.WriteLine(ex.ToString());
					_logger.Log("Server crashed");
					return;
				}
				throw;
			}
			listener.Close();
		}

		public void AcceptCallback(IAsyncResult ar)
		{
			if (_running.Running)
				_logger.Log("Accept Client");
			allDone.Set();
			Socket handler;
			Client client;
			Socket listener = (Socket)ar.AsyncState;
			try
			{
				 handler = listener.EndAccept(ar);
				 client = new Client(handler, this);
			}
			catch (Exception ex)
			{
				if (ex is SocketException || ex is ObjectDisposedException)
				{
					System.Diagnostics.Debug.WriteLine(ex.ToString());
					if (_running.Running)
						_logger.Log("Accept failed !");
					return;
				}
				throw;
			}
			lock (client)
			{
				clientList.Add(client);
			}
		}

		public void DestroyClient(Client client)
		{
			_logger.Log("Destroy Client");
			client.WorkSocket.Shutdown(SocketShutdown.Both);
			client.WorkSocket.Close();
			lock (client)
			{
				clientList.Remove(client);
			}
		}

		public void KickClient(uint clientID)
		{
			Client client = null;
			lock (clientList)
			{
				foreach (var it in clientList)
					if (it.ClientID == clientID)
						client = it;
			}
			if (client != null)
				client.Destroy();
		}
	}
}
