using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Threading;

namespace MineServer
{
	public class World
	{
		private Logger _logger;
		private ConcurrentDictionary<Vector3Int, Chunk> _chunks;
		private ConcurrentDictionary<uint, Player> _players;
		private float _timerResync;
		private const float _TIMEOUT_RESYNC = 2.0f;

		public int SpawnPosX;
		public int SpawnPosY;
		public int SpawnPosZ;

		public enum EWorldType : byte
		{
			SURVIVAL, CREATIVE, ADVENTURE
		};
		public EWorldType WorldType { get; set; }

		public event Action<Player> NotifyPlayerAdd;
		public event Action<Player> NotifyPlayerRemove;

		public World(EWorldType type)
		{
			WorldType = type;
			_logger = Logger.Instance;
			_chunks = new ConcurrentDictionary<Vector3Int, Chunk>();
			_players = new ConcurrentDictionary<uint, Player>();
			SpawnPosX = 0;
			SpawnPosY = 900;
			SpawnPosZ = 0;
			_timerResync = 0.0f;
		}

		public void Update(float dt)
		{
			_timerResync += dt;
			if (_timerResync > _TIMEOUT_RESYNC)
			{
				foreach (var it in _players)
					foreach (var sendTo in _players)
						if (it.Value != sendTo.Value)
							sendTo.Value.SendEntityResyncMove(it.Value);
				_timerResync = 0.0f;
			}
			foreach (var it in _players)
			{
				it.Value.Update(dt);
				foreach (var sendTo in _players)
					if (it.Value != sendTo.Value)
						sendTo.Value.SendEntityMove(it.Value);
			}
			foreach (var it in _players)
				it.Value.ResetOldPos();
		}

		public Chunk[] GetAllChunkInProximity(Vector3Int position, Player player)
		{
			Vector3Int chunkPos = new Vector3Int((int)Math.Floor((double)position.x / 16.0f),
												 (int)Math.Floor((double)position.y / 16.0f),
												 (int)Math.Floor((double)position.z / 16.0f));
			List<Chunk>	chunks = new List<Chunk>();
			for (int i = -5; i <= 5; ++i)
			{
				for (int j = -3; j <= 3; ++j)
				{
					for (int k = -5; k <= 5; ++k)
					{
						Vector3Int	streamChunk = new Vector3Int(chunkPos.x + i, chunkPos.y + j, chunkPos.z + k);
						Chunk		chunk;

						if (player.CheckChunkLoaded(streamChunk))
							continue;
						_chunks.TryGetValue(streamChunk, out chunk);
						if (chunk == null)
						{
							chunk = new Chunk(streamChunk, this);
							_chunks.TryAdd(streamChunk, chunk);
							//_logger.Log("Generating " + streamChunk.x + " " + streamChunk.y + " " + streamChunk.z);
						}
						if (chunk.CheckSum > 0)
							chunks.Add(chunk);
					}
				}
			}
			return chunks.ToArray();
		}

		public Chunk GetChunk(Vector3Int position)
		{
			Vector3Int	chunkPos = new Vector3Int((int)Math.Floor((double)position.x / 16.0f),
												  (int)Math.Floor((double)position.y / 16.0f),
												  (int)Math.Floor((double)position.z / 16.0f));
			Chunk		chunk;

			_chunks.TryGetValue(chunkPos, out chunk);
			if (chunk == null)
			{
				chunk = new Chunk(chunkPos, this);
				_chunks.TryAdd(chunkPos, chunk);
				//_logger.Log("Generating " + chunkPos.x + " " + chunkPos.y);
			}
			return chunk;
		}

		public void AddPlayer(Player player)
		{
			if (NotifyPlayerAdd != null)
				NotifyPlayerAdd(player);
			_players.TryAdd(player.ID, player);
			player.PosX = SpawnPosX;
			player.PosYFeet = SpawnPosY;
			player.PosYHead = SpawnPosY + 1.62d;
			player.PosZ = SpawnPosZ;

			BroadcastSpawnEntity(player);
		}

		public void RemovePlayer(Player player)
		{
			if (NotifyPlayerRemove != null)
				NotifyPlayerRemove(player);
			_players.TryRemove(player.ID, out player);
			BroadcastDestroyEntity(player);
		}

		public void BroadcastMessage(string message, Player emitter)
		{
			foreach (var it in _players)
			{
				it.Value.SendChatMessage(message, emitter);
			}
		}

		public void BroadcastSpawnEntity(Player player)
		{
			foreach (var it in _players)
			{
				if (it.Value != player)
				{
					it.Value.SendSpawnPlayer(player);
					player.SendSpawnPlayer(it.Value);
				}
			}
		}

		public void BroadcastDestroyEntity(Entity entity)
		{
			foreach (var it in _players)
			{
				it.Value.SendDestroyEntity(entity);
			}
		}

		public void BroadcastEntityMetaData(Player from)
		{
			foreach (var it in _players)
			{
				if (from.ID != it.Value.ID)
					it.Value.SendEntityMetadata(from);
			}
		}

		public void BroadcastBlockChange(int x, int y, int z, byte id, byte meta = 0)
		{
			foreach (var it in _players)
				it.Value.SendBlockChange(x, y, z, id, meta);
		}

		public int GetPlayerCount()
		{
			return _players.Count;
		}

		public void UpdateBlockAt(int x, int y, int z, byte id)
		{
			int chunkX = (int)Math.Floor((double)x / 16.0f);
			int chunkY = (int)Math.Floor((double)y / 16.0f);
			int chunkZ = (int)Math.Floor((double)z / 16.0f);

			Chunk		chunk;
			Vector3Int	chunkPos = new Vector3Int(chunkX, chunkY, chunkZ);

			_chunks.TryGetValue(chunkPos, out chunk);
			if (chunk == null)
				return;
			BroadcastBlockChange(x, y, z, id);
			chunk.UpdateBlockAt(x, y, z, id);
		}

		public void RespawnPlayer(Player player)
		{
			player.PosX = SpawnPosX;
			player.PosYFeet = SpawnPosY;
			player.PosYHead = SpawnPosY + 1.62d;
			player.PosZ = SpawnPosZ;
			player.Health = 20;

			BroadcastSpawnEntity(player);
			player.SendRespawn();
		}
	}
}
