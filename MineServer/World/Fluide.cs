using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;

namespace MineServer
{
	/*
	public class Fluide
	{
		private byte _decreasePerBlock;
		private World _world;
		private Queue<Vector3> _waitForUpdate;
		private Queue<Vector3> _waitForUpdateDryout;
		Timer _timer;


		public Fluide(byte decrease, World world)
		{
			_timer = new Timer(200);
			_timer.Elapsed += _timer_Elapsed;
			_timer.Start();
			_decreasePerBlock = decrease;
			_world = world;
			_waitForUpdate = new Queue<Vector3>();
			_waitForUpdateDryout = new Queue<Vector3>();
		}

		void _timer_Elapsed(object sender, ElapsedEventArgs e)
		{
			Update();
		}

		public void Update()
		{
			int size;

			size = _waitForUpdateDryout.Count;
			for (int i = 0; i < size; ++i)
			{
				Vector3 vec = _waitForUpdateDryout.Dequeue();
				UpdateDryout((int)vec.x, (byte)vec.y, (int)vec.z);
			}
			size = _waitForUpdate.Count;
			for (int i = 0; i < size; ++i)
			{
				Vector3 vec = _waitForUpdate.Dequeue();
				UpdateBlockAt((int)vec.x, (byte)vec.y, (int)vec.z);
			}
			if (_waitForUpdateDryout.Count > 0 || _waitForUpdate.Count > 0)
				System.Diagnostics.Debug.WriteLine("DryOut " + _waitForUpdateDryout.Count + "  Update " + _waitForUpdate.Count);
			
		}

		public void AddUpdateDryout(int x, byte y, int z)
		{
			_waitForUpdateDryout.Enqueue(new Vector3(x, (byte)(y - 1), z));
		}

		public void UpdateBlockAt(int x, byte y, int z)
		{
			ChunkColumn chunkData = _world.GetChunkColumn(new Vector2Int(x, z));
			ChunkColumn chunkDataTmp;
			int relX = x < 0 ? 16 + x % 16 : x % 16;
			if (relX == 16)
				relX = 0;
			int relZ = z < 0 ? 16 + z % 16 : z % 16;
			if (relZ == 16)
				relZ = 0;

			byte idTmp;
			byte metaTmp;

			if (chunkData.GetBlockTypeAt(x, y, z) == 9 ||
				chunkData.GetBlockTypeAt(x, y, z) == 8)
			{
				idTmp = chunkData.GetBlockTypeAt(x, (byte)(y - 1), z);
				metaTmp = chunkData.GetMetadataAt(x, (byte)(y - 1), z);
				if (y > 1 && (idTmp == 0 || idTmp == 8))
				{
					if (idTmp == 8 && (metaTmp == 0 || metaTmp == 8))
						return;
					chunkData.HardSetBlockAt(x, (byte)(y - 1), z, 8);
					chunkData.HardSetMetadataAt(x, (byte)(y - 1), z, 8);
					_waitForUpdate.Enqueue(new Vector3(x, (byte)(y - 1), z));
					_world.BroadcastBlockChange(x, (byte)(y - 1), z, chunkData.GetBlockTypeAt(x, (byte)(y - 1), z), 8);
				}
				else
				{
					byte currentMeta = chunkData.GetMetadataAt(x, y, z);
					if (currentMeta == 8)
						currentMeta = 0;

					chunkDataTmp = _world.GetChunkColumn(new Vector2Int(x - 1, z));
					if (chunkDataTmp.GetBlockTypeAt(x - 1, y, z) == 0 || chunkDataTmp.GetBlockTypeAt(x - 1, y, z) == 8)
					{
						UpdateNextBlock(x - 1, y, z, chunkDataTmp, (byte)(currentMeta + _decreasePerBlock));
					}
					chunkDataTmp = _world.GetChunkColumn(new Vector2Int(x + 1, z));
					if (chunkDataTmp.GetBlockTypeAt(x + 1, y, z) == 0 || chunkDataTmp.GetBlockTypeAt(x + 1, y, z) == 8)
					{
						UpdateNextBlock(x + 1, y, z, chunkDataTmp, (byte)(currentMeta + _decreasePerBlock));
					}
					chunkDataTmp = _world.GetChunkColumn(new Vector2Int(x, z - 1));
					if (chunkDataTmp.GetBlockTypeAt(x, y, z - 1) == 0 || chunkDataTmp.GetBlockTypeAt(x, y, z - 1) == 8)
					{
						UpdateNextBlock(x, y, z - 1, chunkDataTmp, (byte)(currentMeta + _decreasePerBlock));
					}
					chunkDataTmp = _world.GetChunkColumn(new Vector2Int(x, z + 1));
					if (chunkDataTmp.GetBlockTypeAt(x, y, z + 1) == 0 || chunkDataTmp.GetBlockTypeAt(x, y, z + 1) == 8)
					{
						UpdateNextBlock(x, y, z + 1, chunkDataTmp, (byte)(currentMeta + _decreasePerBlock));
					}
				}
			}
		}

		private void UpdateDryout(int x, byte y, int z)
		{
			ChunkColumn chunkData = _world.GetChunkColumn(new Vector2Int(x, z));
			
			int relX = x < 0 ? 16 + x % 16 : x % 16;
			if (relX == 16)
				relX = 0;
			int relZ = z < 0 ? 16 + z % 16 : z % 16;
			if (relZ == 16)
				relZ = 0;

			byte currentId = chunkData.GetBlockTypeAt(x, y, z);
			byte currentMeta = chunkData.GetMetadataAt(x, y, z);
		
			if (currentMeta == 8 && currentId == 8)
				currentMeta = 0;

			UpdateNextDryout(x, y, z, currentId, currentMeta);
			UpdateNextDryout(x, (byte)(y - 1), z, currentId, 0);
			UpdateNextDryout(x - 1, y, z, currentId, currentMeta, false);
			UpdateNextDryout(x + 1, y, z, currentId, currentMeta, false);
			UpdateNextDryout(x, y, z - 1, currentId, currentMeta, false);
			UpdateNextDryout(x, y, z + 1, currentId, currentMeta, false);
			
			currentId = chunkData.GetBlockTypeAt(x, (byte)(y + 1), z);
			if (currentId == 8 || currentId == 9)
				_waitForUpdate.Enqueue(new Vector3(x, (byte)(y + 1), z));
		}

		private void UpdateNextDryout(int x, byte y, int z, byte id, byte meta = 0, bool fallingUpdatable = true)
		{
			ChunkColumn chunkDataTmp = _world.GetChunkColumn(new Vector2Int(x, z));
			byte idTmp = chunkDataTmp.GetBlockTypeAt(x, y, z);
			byte metaTmp = chunkDataTmp.GetMetadataAt(x, y, z);
			byte idTmpDown = chunkDataTmp.GetBlockTypeAt(x, (byte)(y - 1), z);
			byte metaTmpDown = chunkDataTmp.GetMetadataAt(x, (byte)(y - 1), z);

			if ( (fallingUpdatable == true || metaTmp != 8) && idTmp == 8 && metaTmp >= meta && metaTmp > 0)
			{
				byte tmp = (byte)(metaTmp + _decreasePerBlock);
				if (tmp >= 8 || (idTmpDown == 8 && metaTmpDown > 0))
				{
					chunkDataTmp.HardSetBlockAt(x, y, z, 0);
					chunkDataTmp.HardSetMetadataAt(x, y, z, 0);
				}
				else
				{
					chunkDataTmp.HardSetMetadataAt(x, y, z, tmp);
				}
				_waitForUpdateDryout.Enqueue(new Vector3(x, y, z));
				_world.BroadcastBlockChange(x, y, z, chunkDataTmp.GetBlockTypeAt(x, y, z), chunkDataTmp.GetMetadataAt(x, y, z));
			}
			//else if (idTmp == 9 || (idTmp == 8 && (metaTmp == 8 || metaTmp < meta)))
			//{
			//	_waitForUpdate.Enqueue(new Vector3(x, y, z));
			//}
		}

		private void UpdateNextBlock(int x, byte y, int z, ChunkColumn chunkData, byte metadata)
		{
			if (metadata >= 0x08)
				return;
			if (chunkData.GetBlockTypeAt(x, y, z) == 0)
			{
				chunkData.HardSetBlockAt(x, y, z, 8);
				chunkData.HardSetMetadataAt(x, y, z, metadata);
			}
			else if (chunkData.GetBlockTypeAt(x, y, z) == 8)
			{
				if (chunkData.GetMetadataAt(x, y, z) > metadata &&
					chunkData.GetMetadataAt(x, y, z) != 0x08)
					chunkData.HardSetMetadataAt(x, y, z, metadata);
				else
					return;
			}
			_waitForUpdate.Enqueue(new Vector3(x, y, z));
			_world.BroadcastBlockChange(x, y, z, chunkData.GetBlockTypeAt(x, y, z), metadata);
		}
	}
	 */
}
