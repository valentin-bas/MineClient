#pragma once

#include "ByteBuffer.h"
#include "Client.h"

#define SIZE_PACKET 256

namespace MineClient
{
	namespace Network
	{
		class Packetizer : public cByteBuffer
		{
		public:
			Packetizer(int id, Client* client);
			~Packetizer(void);
		private:
			Client*	_client;
			int		_id;
		};
	}
}