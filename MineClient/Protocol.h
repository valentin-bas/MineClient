#pragma once

#include "Client.h"
#include "MineEngine.h"

namespace MineClient
{
	namespace Network
	{
		class Protocol
		{
		public:
			Protocol(void);
			~Protocol(void);

			static void HandleReadBuffer(Client& client, CMineEngine& engine, id::Obj* steve);
			//----- Send -----------------------------
			static void SendHandshake(Client& client);
			static void SendLoginStart(Client& client);
			static void SendPlayerPosition(Client& client, double x, double y, double z, double pitch, double yaw);
			static void SendBlockPlacment(Client& client, glm::vec4& block, glm::vec3& cursor);
			static void	SendDig(Client& client, char status, glm::vec4& blockPos);
		private:
			//-----Handle-----------------------------
			//State == Connected

			//State == Loggin
			static void HandleJoinGame(Client&  client, cByteBuffer& packet);			// 0x01
			static void HandleLoginSuccess(Client&  client, cByteBuffer& packet);		// 0x02

			//State == Game
			static void HandleKeepAlive(Client& client, cByteBuffer& packet);											//0x00
			static void HandleChatMessage(Client& client, cByteBuffer& packet);											//0x02
			static void HandlePlayer(Client&  client, cByteBuffer& packet);												//0x03
			static void HandleSpawnPosition(Client&  client, cByteBuffer& packet);										//0x05
			static void HandleUpdateHealth(Client&  client, cByteBuffer& packet);										//0x06
			static void HandleRespawn(Client&  client, cByteBuffer& packet);											//0x07
			static void HandlePlayerPosAndLook(Client&  client, cByteBuffer& packet);									//0x08
			static void HandleDestroyEntity(Client&  client, cByteBuffer& packet);										//0x13
			static void HandleEntityRelativeMove(Client&  client, cByteBuffer& packet);									//0x15
			static void HandleEntityLook(Client&  client, cByteBuffer& packet);											//0x16
			static void HandleEntityRelativeMoveAndLook(Client&  client, cByteBuffer& packet, CMineEngine& engine);		//0x17
			static void HandleEntityTeleport(Client&  client, cByteBuffer& packet, CMineEngine& engine);				//0x18
			static void HandleEntityHeadLook(Client&  client, cByteBuffer& packet);										//0x19
			static void HandleBlockChange(Client&  client, cByteBuffer& packet, CMineEngine& engine);					//0x23
			static void HandleExplosion(Client&  client, cByteBuffer& packet);											//0x27
			static void HandleAnimation(Client&  client, cByteBuffer& packet);											//0x0b
			static void HandleSpawnPlayer(Client&  client, cByteBuffer& packet, CMineEngine& engine, id::Obj* steve);	//0x0c
			static void HandleSpawnObject(Client&  client, cByteBuffer& packet);										//0x0e
			static void HandleEntityMetadata(Client&  client, cByteBuffer& packet);										//0x1c
			static void HandleChunk(Client&  client, cByteBuffer& packet, CMineEngine& engine);							//0x21
			static void HandleWindowItem(Client&  client, cByteBuffer& packet);											//0x30
			static void HandleSetExperience(Client&  client, cByteBuffer& packet);										//0x1F
			static void HandlePlayerAbility(Client&  client, cByteBuffer& packet);										//0x39
		};
	}
}