using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MineServer
{
	public class Packetizer : BytesBuffer
	{
		private Client _client;
		private BytesBuffer _sendBuffer;

		public Packetizer(Client client)
		{
			_client = client;
			_sendBuffer = new BytesBuffer();
		}

		public void End()
		{
			_sendBuffer.WriteVarInt((int)base.Length);
			_sendBuffer.Write(base.GetBuffer(), 0, (int)base.Length);
			base.SetLength(0);
			base.Position = 0;
		}

		public void Send()
		{
			_client.Send(_sendBuffer.GetBuffer(), 0, (int)_sendBuffer.Length);
		}
	}
}
