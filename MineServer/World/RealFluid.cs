using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;

namespace MineServer
{
	/*
	public class RealFluid
	{
		public class CubeFluid
		{
			public int x;
			public byte y;
			public int z;
			public float level;

			public CubeFluid(int px, byte py, int pz, float plevel)
			{
				x = px;
				y = py;
				z = pz;
				level = plevel;
			}
		}

		private Dictionary<Vector3, CubeFluid> _cubes;
		private List<Vector3> _nextWaitUpdate;
		private List<Vector3> _waitForUpdate;
		private ConcurrentQueue<CubeFluid> _nextWaitAdd;
		private ConcurrentQueue<CubeFluid> _waitForAdd;
		private World _world;
		Timer _timer;

		public RealFluid(World world)
		{
			_waitForUpdate = new List<Vector3>();
			_nextWaitUpdate = new List<Vector3>();
			_waitForAdd = new ConcurrentQueue<CubeFluid>();
			_nextWaitAdd = new ConcurrentQueue<CubeFluid>();
			_timer = new Timer(200);
			_timer.Elapsed += _timer_Elapsed;
			_timer.Start();
			_world = world;
			_cubes = new Dictionary<Vector3, CubeFluid>();
		}

		void _timer_Elapsed(object sender, ElapsedEventArgs e)
		{
			Update();
		}

		public void Update()
		{
			CubeFluid cube;
			int size = _waitForUpdate.Count;
			Vector3 tmp;
			CubeFluid tmpCube;
			for (int i = 0; i < size; ++i)
			{
				try
				{
					cube = _cubes[_waitForUpdate[i]];
					UpdateCube(cube);
				}
				catch (KeyNotFoundException)
				{}
			}
			_waitForUpdate = _nextWaitUpdate;
			_nextWaitUpdate.Clear();
			size = _waitForAdd.Count;
			for (int i = 0; i < size; ++i)
			{
				if (_waitForAdd.TryDequeue(out tmpCube))
					AddCube(tmpCube);
			}
			_waitForAdd = _nextWaitAdd;
		}

		public void UpdateCube(CubeFluid cube)
		{
			CubeFluid cubeTmp;
			bool cubeInfo;
			byte idTmp;
			float levelToAdd = 0.0f;

			cubeInfo = GetIdAndCube(cube.x, (byte)(cube.y - 1), cube.z, out idTmp, out cubeTmp);
			if (idTmp == 0 || (idTmp == 8 && cubeInfo == true && cubeTmp.level < 0.95f))
			{
				if (idTmp == 0)
					levelToAdd = Math.Min(cube.level, 0.5f);
				else
					levelToAdd = Math.Max(Math.Min(Math.Min(1.0f - cubeTmp.level, cube.level), 0.5f), 0.0f);
				if (levelToAdd < 0.001f)
					return;
				_nextWaitAdd.Enqueue(new CubeFluid(cube.x, cube.y, cube.z, -levelToAdd));
				_nextWaitAdd.Enqueue(new CubeFluid(cube.x, (byte)(cube.y - 1), cube.z, levelToAdd));
			}
			else
			{
				UpdateSide(cube, -1, 0);
				UpdateSide(cube, 1, 0);
				UpdateSide(cube, 0, -1);
				UpdateSide(cube, 0, 1);
			}
			
			HardSetAndBroadcast(cube);
		}

		public void UpdateSide(CubeFluid cube, int difX, int difZ)
		{
			CubeFluid cubeTmp;
			bool cubeInfo;
			byte idTmp;
			float levelToAdd = 0.0f;

			cubeInfo = GetIdAndCube(cube.x + difX, cube.y, cube.z + difZ, out idTmp, out cubeTmp);
			if (idTmp == 0 || (idTmp == 8 && cubeInfo == true))
			{
				levelToAdd = Math.Max(Math.Min((cube.level - cubeTmp.level) / 8.0f, 0.2f), -0.2f);

				if (levelToAdd > -0.001f && levelToAdd < 0.001f)
					return;
				_nextWaitAdd.Enqueue(new CubeFluid(cube.x, cube.y, cube.z, -levelToAdd));
				_nextWaitAdd.Enqueue(new CubeFluid(cube.x + difX, cube.y, cube.z + difZ, levelToAdd));
			}
		}

		public bool GetIdAndCube(int x, byte y, int z, out byte id, out CubeFluid cube)
		{
			ChunkColumn chunkData = _world.GetChunkColumn(new Vector2Int(x, z));

			id = chunkData.GetBlockTypeAt(x, y, z);
			cube = new CubeFluid(x, y, z, 0.0f);
			try
			{
				cube = _cubes[new Vector3(x, y, z)];
			}
			catch (KeyNotFoundException)
			{
				return false;
			}
			return true;
		}

		public void AddCube(CubeFluid cube)
		{
			Vector3 pos = new Vector3(cube.x, cube.y, cube.z);
			CubeFluid cubeRef;
			string str = "";

			try
			{
				cubeRef = _cubes[pos];
				cubeRef.level += cube.level;
				str += "Update ";
			}
			catch (KeyNotFoundException)
			{
				_cubes.Add(pos, cube);
				cubeRef = _cubes[pos];
				str += "Add ";
			}
			cubeRef.level = Math.Max(Math.Min(cubeRef.level, 1.0f), 0.0f);
			if (cubeRef.level < 0.001f)
			{
				RmCube(cubeRef, true);
				return;
			}

			AddNextUpdate(pos);
		}

		public void RmCube(CubeFluid cube, bool needToBroadcast)
		{
			if (needToBroadcast)
			{
				cube.level = 0.0f;
				HardSetAndBroadcast(cube);
			}
			else
			{
				AddNextUpdate(new Vector3(cube.x - 1, cube.y, cube.z));
				AddNextUpdate(new Vector3(cube.x + 1, cube.y, cube.z));
				AddNextUpdate(new Vector3(cube.x, cube.y, cube.z - 1));
				AddNextUpdate(new Vector3(cube.x, cube.y, cube.z + 1));
				AddNextUpdate(new Vector3(cube.x, (byte)(cube.y + 1), cube.z));
			}
			_cubes.Remove(new Vector3(cube.x, cube.y, cube.z));
		}

		public void AddNextUpdate(Vector3 pos)
		{
			if (_nextWaitUpdate.Contains(pos) == false)
				_nextWaitUpdate.Add(pos);
		}

		public void HardSetAndBroadcast(CubeFluid cube)
		{
			ChunkColumn chunk = _world.GetChunkColumn(new Vector2Int(cube.x, cube.z));
			CubeFluid upCube;
			byte id;
			byte meta;


			id = 8;
			if (cube.level < 0.001f)
				id = 0;
			meta = (byte)Math.Min(((1.0f - cube.level) * 7), 7);
			try
			{
				upCube = _cubes[new Vector3(cube.x, (byte)(cube.y + 1), cube.z)];
				if (upCube.level > 0.05f)
					meta = 8;
			}
			catch (KeyNotFoundException){}
			chunk.HardSetBlockAt(cube.x, cube.y, cube.z, id);
			chunk.HardSetMetadataAt(cube.x, cube.y, cube.z, meta);
			if (id == 8)
				id = 35;
			_world.BroadcastBlockChange(cube.x, cube.y, cube.z, id, meta);
		}
	}
	 */
}
