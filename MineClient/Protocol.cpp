#include "Globals.h"
#include "Protocol.h"
#include "Packetizer.h"
#include "zlib.h"

using namespace MineClient::Network;

Protocol::Protocol(void)
{
}


Protocol::~Protocol(void)
{
}


void
Protocol::HandleReadBuffer(Client& client, CMineEngine& engine, id::Obj* steve)
{
	cByteBuffer*	stream = client.TryLockReadBuf();

	if (stream == nullptr)
		return;
	while (stream->GetReadableSpace() != 0)
	{
		UINT32			datalen, header;
		Int32			state = client.GetState();
		cByteBuffer		packet(2048);

		if (!stream->ReadVarInt(datalen))
		{
			stream->ResetRead();
			break;
		}
		if (!stream->CanReadBytes(datalen))
		{
			stream->ResetRead();
			break;
		}
		char*	tmpBuf = new (std::nothrow) char[datalen];
		if (tmpBuf == nullptr)
		{
			client.ReleaseReadBuf();
			return; //false
		}
		stream->ReadBuf(tmpBuf, datalen);
		packet.Write(tmpBuf, datalen);
		free(tmpBuf);
		packet.ReadVarInt(header);
		HandleKeepAlive(client, packet);
		switch (state)
		{
		case States::CONNECT :
			break;
		case States::LOGGING :
				 if (header == 0x01)	HandleJoinGame(client, packet);
			else if (header == 0x02)	HandleLoginSuccess(client, packet);
			break;
		case States::GAME :
				 if (header == 0x00)	HandleKeepAlive					(client, packet);
			else if (header == 0x02)	HandleChatMessage				(client, packet);
			else if	(header == 0x03)	HandlePlayer					(client, packet);
			else if	(header == 0x05)	HandleSpawnPosition				(client, packet);
			else if	(header == 0x06)	HandleUpdateHealth				(client, packet);
			else if	(header == 0x07)	HandleRespawn					(client, packet);
			else if	(header == 0x08)	HandlePlayerPosAndLook			(client, packet);
			else if	(header == 0x13)	HandleDestroyEntity				(client, packet);
			else if	(header == 0x15)	HandleEntityRelativeMove		(client, packet);
			else if	(header == 0x16)	HandleEntityLook				(client, packet);
			else if	(header == 0x17)	HandleEntityRelativeMoveAndLook	(client, packet, engine);
			else if	(header == 0x18)	HandleEntityTeleport			(client, packet, engine);	
			else if	(header == 0x19)	HandleEntityHeadLook			(client, packet);
			else if	(header == 0x23)	HandleBlockChange				(client, packet, engine);
			else if	(header == 0x27)	HandleExplosion					(client, packet);
			else if	(header == 0x0b)	HandleAnimation					(client, packet);
			else if	(header == 0x0c)	HandleSpawnPlayer				(client, packet, engine, steve);
			else if	(header == 0x0e)	HandleSpawnObject				(client, packet);
			else if	(header == 0x1c)	HandleEntityMetadata			(client, packet);
			else if	(header == 0x21)	HandleChunk						(client, packet, engine);
			else if	(header == 0x30)	HandleWindowItem				(client, packet);
			else if	(header == 0x1F)	HandleSetExperience				(client, packet);
			else if	(header == 0x39)	HandlePlayerAbility				(client, packet);
			break;
		default:
			break;
		}
		stream->CommitRead();
	}
	client.ReleaseReadBuf();
}

//-----Handle----------------------------------s
//State == Connected

//State == Loggin
void Protocol::HandleJoinGame(Client&  client, cByteBuffer& packet)// 0x01
{
	client.SetState(MineClient::Network::GAME);
}

void Protocol::HandleLoginSuccess(Client&  client, cByteBuffer& packet)// 0x02
{
}

//State == Game
void Protocol::HandleKeepAlive(Client& client, cByteBuffer& packet)// 0x00
{
}

void Protocol::HandleChatMessage(Client& client, cByteBuffer& packet)// 0x02
{
}

void Protocol::HandlePlayer(Client&  client, cByteBuffer& packet)// 0x03
{
}

void Protocol::HandleSpawnPosition(Client&  client, cByteBuffer& packet)// 0x05
{
}

void Protocol::HandleUpdateHealth(Client&  client, cByteBuffer& packet)// 0x06
{
}

void Protocol::HandleRespawn(Client&  client, cByteBuffer& packet)// 0x07
{
}

void Protocol::HandlePlayerPosAndLook(Client&  client, cByteBuffer& packet)// 0x08
{
}

void Protocol::HandleDestroyEntity(Client&  client, cByteBuffer& packet)//0x13
{
}

void Protocol::HandleEntityRelativeMove(Client&  client, cByteBuffer& packet)//0x15
{
}

void Protocol::HandleEntityLook(Client&  client, cByteBuffer& packet)//0x16
{
}

void Protocol::HandleEntityRelativeMoveAndLook(Client&  client, cByteBuffer& packet, CMineEngine& engine)//0x17
{
	int	id;
	unsigned char dx, dy, dz, yaw, pitch;
	glm::vec3	pos;
	glm::vec3	rot;

	packet.ReadBEInt(id);
	packet.ReadByte(dx);
	packet.ReadByte(dy);
	packet.ReadByte(dz);
	packet.ReadByte(yaw);
	packet.ReadByte(pitch);
	engine.GetEntityTransform(id, pos, rot);
	pos.x += ((float)(char)dx / 32) * 2;
	pos.y += ((float)(char)dy / 32) * 2;
	pos.z += ((float)(char)dz / 32) * 2;
	rot.x = 0.0f;
	rot.y = -((float)yaw / 255.0f) * 360.0f - 90.0f;
	rot.z = 0.0f;
	engine.SetEntityTransform(id, pos, rot);
}

void Protocol::HandleEntityTeleport(Client&  client, cByteBuffer& packet, CMineEngine& engine)//0x18
{
	int id, x, y, z;
	unsigned char yaw, pitch;

	packet.ReadBEInt(id);
	packet.ReadBEInt(x);
	packet.ReadBEInt(y);
	packet.ReadBEInt(z);
	packet.ReadByte(yaw);
	packet.ReadByte(pitch);
	glm::vec3	pos;
	pos.x = ((float)x / 32.0f) * 2.0f;
	pos.y = ((float)y / 32.0f - 1.82f) * 2.0f;
	pos.z = ((float)z / 32.0f) * 2.0f;
	glm::vec3	rot;
	rot.x = 0.0f;
	rot.y = -((float)yaw / 255.0f) * 360.0f - 90.0f;
	rot.z = 0.0f;
	engine.SetEntityTransform(id, pos, rot);
}

void Protocol::HandleEntityHeadLook(Client&  client, cByteBuffer& packet)//0x19
{
}

void Protocol::HandleBlockChange(Client&  client, cByteBuffer& packet, CMineEngine& engine)//0x23
{
	int				x, y, z;
	int				id;
	unsigned char	meta;

	packet.ReadBEInt(x);
	packet.ReadBEInt(y);
	packet.ReadBEInt(z);
	packet.ReadVarInt(id);
	packet.ReadByte(meta);
	engine.UpdateBlockChange(x, y, z, id);
}

void Protocol::HandleExplosion(Client&  client, cByteBuffer& packet)//0x27
{
}

void Protocol::HandleAnimation(Client&  client, cByteBuffer& packet)//0x0b
{
}

void Protocol::HandleSpawnPlayer(Client&  client, cByteBuffer& packet, CMineEngine& engine, id::Obj* steve)//0x0c
{
	int	id, x, y, z;
	unsigned char yaw, pitch;
	AString	str;

	packet.ReadVarInt(id);
	packet.ReadVarUTF8String(str);
	packet.ReadVarUTF8String(str);
	packet.ReadBEInt(x);
	packet.ReadBEInt(y);
	packet.ReadBEInt(z);
	packet.ReadByte(yaw);
	packet.ReadByte(pitch);
	if (engine.LoadEntityFromObj(*steve, id) < 0)
		return; //false
	glm::vec3	pos;
	pos.x = ((double)x / 32.0f) * 2.0f;
	pos.y = ((double)y / 32.0f - 1.82f) * 2.0f;
	pos.z = ((double)z / 32.0f) * 2.0f;
	glm::vec3	rot;
	rot.x = 0.0f;
	rot.y = -((double)yaw / 255.0f) * 360.0f - 90.0f;
	rot.z = 0.0f;
	engine.SetEntityTransform(id, pos, rot);
}

void Protocol::HandleSpawnObject(Client&  client, cByteBuffer& packet)//0x0e
{
}

void Protocol::HandleEntityMetadata(Client&  client, cByteBuffer& packet)//0x1c
{

}

void Protocol::HandleChunk(Client&  client, cByteBuffer& packet, CMineEngine& engine)// 0x21
{
	int x, z, y, compressSize;
	short primaryMask, addMask;
	packet.ReadBEInt(x);
	packet.ReadBEInt(y);
	packet.ReadBEInt(z);
	packet.SkipRead(sizeof(bool));
	packet.ReadLEShort(primaryMask);
	packet.SkipRead(sizeof(short));
	packet.ReadBEInt(compressSize);
	if (primaryMask != 0)
	{
		Bytef* compressData;
		compressData = new (std::nothrow) Bytef[compressSize];
		if (compressData == nullptr)
			return; //false
		packet.ReadBuf(compressData, compressSize);
		engine.LoadChunk(x, y, z, compressData, compressSize);
	}
	else
		engine.UnloadChunk(x, y, z);
	return; //true
}

void Protocol::HandleWindowItem(Client&  client, cByteBuffer& packet)// 0x30
{
}

void Protocol::HandleSetExperience(Client&  client, cByteBuffer& packet)// 0x1F
{
}

void Protocol::HandlePlayerAbility(Client&  client, cByteBuffer& packet)// 0x39
{
}


//----- Send ----------------------------------

void	Protocol::SendHandshake(Client& client)
{
	Packetizer	packet(0x00, &client);

	packet.WriteVarInt(4);
	packet.WriteVarUTF8String(client.GetServer());
	packet.WriteBEShort(std::stoi(client.GetPort()));
	packet.WriteVarInt(2);
}

void	Protocol::SendLoginStart(Client& client)
{
	Packetizer	packet(0x00, &client);

	packet.WriteVarUTF8String(client.GetName());
	client.SetState(MineClient::Network::LOGGING);
}

void	Protocol::SendPlayerPosition(Client& client, double x, double y, double z,
									 double pitch, double yaw)
{
	Packetizer	packet(0x06, &client);

	packet.WriteBEDouble(x);
	packet.WriteBEDouble(y - 1.0f);
	packet.WriteBEDouble(y);
	packet.WriteBEDouble(z);
	packet.WriteBEFloat(((float)yaw / 360.0f) * 255);
	packet.WriteBEFloat(((float)pitch / 360.0f) * 255);
	packet.WriteBool(false);
}

void	Protocol::SendBlockPlacment(Client& client, glm::vec4& block, glm::vec3& cursor)
{
	Packetizer	packet(0x08, &client);

	packet.WriteBEInt(block.x);
	packet.WriteByte(block.y);
	packet.WriteBEInt(block.z);
	packet.WriteByte(block.w);
	//--Slot
	packet.WriteBEShort(1);
	packet.WriteByte(1);
	packet.WriteBEShort(0);
	packet.WriteBEShort(0);
	//--
	packet.WriteByte(cursor.x);
	packet.WriteByte(cursor.y);
	packet.WriteByte(cursor.z);
}

void	Protocol::SendDig(Client& client, char status, glm::vec4& blockPos)
{
	Packetizer packet(0x07, &client);

	packet.WriteByte(status);
	packet.WriteBEInt(blockPos.x);
	packet.WriteByte(blockPos.y);
	packet.WriteBEInt(blockPos.z);
	packet.WriteByte(blockPos.w);
}