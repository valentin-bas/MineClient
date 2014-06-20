using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MineServer
{
	public class Player : Entity
	{
		private Client	_client;
		private Protocol _protocol;
		private float _timerKeepAlive = 0.0f;
		private const float D_TIME_KEEP_ALIVE = 1.0f;
		
		public string Name { get; set; }
		private uint _UUID;
		public	uint UUID { get { return _UUID; } }
		private static uint _nextUUID = 0;

		private bool _isCrouched = false;
		public bool isCrouched { get { return _isCrouched; } }
		private bool _isOnFire = false;
		private bool _isSprinting = false;
		public bool isSrinting { get { return _isSprinting; } }
		private bool _isInvisible = false;

		private Vector3Int _lastChunk;
		private List<Vector3Int> _loadedChunk;

		public int Health { get; set; }

		public Player(Client client, Protocol protocol)
		{
			Health = 20;
			_client = client;
			_UUID = _nextUUID;
			_nextUUID++;
			Logger.Instance.Log("New Player ! UUID = " + UUID + " - ID = " + ID);
			PosX = 10.0d;
			PosYFeet = 128.0d;
			PosYHead = 168.0d + 1.62;
			PosZ = 10.0d;
			Pitch = 1.0f;
			Yaw = 1.0f;
			OnGround = false;
			_lastChunk = new Vector3Int();
			_loadedChunk = new List<Vector3Int>();
			_protocol = protocol;
		}

		public uint GetClientID()
		{
			return _client.ClientID;
		}

		public void Update(float dt)
		{
			_timerKeepAlive += dt;
			if (_timerKeepAlive > D_TIME_KEEP_ALIVE)
			{
				SendKeepAlive();
				_timerKeepAlive = 0.0f;
			}
		}

		public void ApplyAction(byte action)
		{
			if (action == 1)
				_isCrouched = true;
			else if (action == 2)
				_isCrouched = false;
			else if (action == 4)
				_isSprinting = true;
			else if (action == 5)
				_isSprinting = false;
		}

		public bool CheckChunkLoaded(Vector3Int chunkPos)
		{
			if (_loadedChunk.Contains(chunkPos))
				return true;
			return false;
		}

		public bool CheckChangedChunk()
		{
			Vector3Int newChunk = new Vector3Int((int)(PosX / 16.0f), (int)(PosY / 16.0f), (int)(PosZ / 16.0f));

			if (newChunk != _lastChunk)
			{
				_lastChunk.x = newChunk.x;
				_lastChunk.y = newChunk.y;
				_lastChunk.z = newChunk.z;
				return true;
			}
			return false;
		}

		public void SendKeepAlive()
		{
			if (_protocol.State == 3)
			{
				Packetizer packet = new Packetizer(_client);
				_protocol.FillKeepAlive(packet);
				packet.Send();
			}
		}

		public void SendAllChunkInProximity(Chunk[] chunkInProximity)
		{
			int count = 0 ;
			Packetizer packet = new Packetizer(_client);

			foreach (Chunk chunk in chunkInProximity)
			{
				++count;
				_protocol.FillChunk(packet, chunk);
				_loadedChunk.Add(chunk.position);
			}
			Vector3Int pos = new Vector3Int((int)PosX, (int)PosY, (int)PosZ);
			List<Vector3Int> toRm = new List<Vector3Int>();
			foreach (Vector3Int vec in _loadedChunk)
			{
				Vector3Int testDist = new Vector3Int(vec.x * 16, vec.y * 16, vec.z * 16);
				double dist = pos.Distance(testDist);
				if (dist >= 160.0d)
				{
					++count;
					toRm.Add(vec);
					_protocol.FillUnloadChunk(packet, vec);
				}
			}
			foreach (var it in toRm)
				_loadedChunk.Remove(it);
			if (count > 0)
				packet.Send();
		}

		public void SendSpawnPlayer(Player player)
		{
			Packetizer packet = new Packetizer(_client);

			_protocol.FillSpawnPlayer(packet, player);
			packet.Send();
		}

		public void SendEntityMove(Entity entity)
		{
			Packetizer packet = new Packetizer(_client);

			double dtX = entity.RelX;
			double dtY = entity.RelY;
			double dtZ = entity.RelZ;
			if (dtX > 4.0d || dtX < -4.0d ||
				dtY > 4.0d || dtY < -4.0d ||
				dtZ > 4.0d || dtZ < -4.0d)
				_protocol.FillEntityTeleport(packet, entity);
			else
			{
				_protocol.FillEntityRelativeMoveLook(packet, entity);
				_protocol.FillEntityHeadLook(packet, entity);
			}
			packet.Send();
		}

		public void SendEntityResyncMove(Entity entity)
		{
			Packetizer packet = new Packetizer(_client);

			_protocol.FillEntityTeleport(packet, entity);
			packet.Send();
		}

		public void SendBlockChange(int x, int y, int z, byte id, byte meta = 0)
		{
			Packetizer packet = new Packetizer(_client);

			_protocol.FillBlockChange(packet, x, y, z, id, meta);
			packet.Send();
		}


		public void SendAnimation(int Id, byte animationType)
		{
			Packetizer packet = new Packetizer(_client);

			_protocol.FillAnimation(packet, Id, animationType);
			packet.Send();
		}

		public void SendEntityMetadata(Player from)
		{
			Packetizer packet = new Packetizer(_client);

			_protocol.FillEntityMetadata(packet, from);
			packet.Send();
		}

		public void SendDestroyEntity(Entity entity)
		{
			Packetizer packet = new Packetizer(_client);

			_protocol.FillDestroyEntity(packet, entity);
			packet.Send();
		}

		public void SendChatMessage(string message, Player emitter)
		{
			Packetizer packet = new Packetizer(_client);

			_protocol.FillChatMessage(packet, emitter, message);
			packet.Send();
		}

		/*public void SendFallingBlock(FallingBlock block)
		{
			Packetizer packet = new Packetizer(_client);

			_protocol.FillSpawnObject(packet, block);
			packet.Send();
		}*/

		public void SendUpdateHealth()
		{
			Packetizer packet = new Packetizer(_client);

			_protocol.FillUpdateHealth(packet, this);
			packet.Send();
		}

		public void SendRespawn()
		{
			Packetizer packet = new Packetizer(_client);

			_protocol.FillRespawn(packet);
			_protocol.FillPlayerPositionAndLook(packet, this);
			packet.Send();
		}

		public void SendExplosion(Explosion explo)
		{
			Packetizer packet = new Packetizer(_client);

			_protocol.FillExplosion(packet, explo);
			packet.Send();
		}
	}
}
