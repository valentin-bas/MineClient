#include "Globals.h"
#include "Packetizer.h"

using namespace MineClient::Network;

Packetizer::Packetizer(int id, Client* client) :
	_id(id), cByteBuffer(SIZE_PACKET)
{
	_client = client;
	WriteVarInt(_id);
}


Packetizer::~Packetizer(void)
{
	if (_client != nullptr)
	{
		AString	data;
		int len = GetReadableSpace();
		ReadAll(data);
		CommitRead();
		WriteVarInt(len);
		Write(data.data(), len);
		ReadAll(data);
		_client->WriteToSendBuf(data.data(), data.length());
	}
}
