using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ICSharpCode.SharpZipLib.Zip.Compression;
using System.Collections.Concurrent;

namespace MineServer
{
	public class Chunk
	{
		public const int Width = 16;
		public const int Height = 16;
		public const int NumBlocks = Width * Height * Width;

		public const int DataSize = sizeof(byte) * NumBlocks;
		private byte[] chunkData = new byte[DataSize];

		public Vector3Int position;
		private World _world;

		private int _checkSum;
		public int CheckSum { get { return _checkSum; } }

		public Chunk(Vector3Int position, World world)
		{
			_checkSum = 0;
			this.position = position;
			_world = world;
			GenerateNoiseWorld();
		}

		public byte[] GetCompressData(out int length)
		{
			lock (this)
			{
				byte[] compressData = new byte[DataSize];
				ConcurrentStack<Deflater> DeflaterPool = new ConcurrentStack<Deflater>();
				Deflater deflater;
				DeflaterPool.TryPop(out deflater);
				if (deflater == null)
					deflater = new Deflater(5);
				deflater.SetInput(chunkData, 0, DataSize);
				deflater.Finish();
				length = deflater.Deflate(compressData);
				deflater.Reset();
				DeflaterPool.Push(deflater);
				return compressData;
			}
		}

		public void UpdateBlockAt(int x, int y, int z, byte id)
		{
			lock (this)
			{
				int relX = x < 0 ? 16 + x % 16 : x % 16;
				if (relX == 16)
					relX = 0;
				int relZ = z < 0 ? 16 + z % 16 : z % 16;
				if (relZ == 16)
					relZ = 0;
				relZ *= 16;
				int relY = y < 0 ? 16 + y % 16 : y % 16;
				if (relY == 16)
					relY = 0;
				relY *= 256;
				_checkSum -= chunkData[relX + relZ + relY];
				_checkSum += id;
				chunkData[relX + relZ + relY] = id;
			}
		}

		public void GenerateNoiseWorld()
		{
			double radius = 200000;
			double elevation = 12000;
			lock (this)
			{
				float[] tab = new float[Width * Width * Width];
				for (int y = 0; y < Width; ++y)
				{
					for (int z = 0; z < Width; ++z)
					{
						for (int x = 0; x < Width; ++x)
							tab[x + z * Width + y * Width * Width] = Noise.Noise.GetOctaveNoise(((position.x) * 16.0f + x) / 32.0f, ((position.y) * 16.0f + y) / 32.0f, ((position.z) * 16.0f + z) / 32.0f, 2);
					}
				}
				for (int y = 0; y < Width; ++y)
				{
					for (int z = 0; z < Width; ++z)
					{
						for (int x = 0; x < Width; ++x)
						{
							double distToCenter = Math.Pow(position.x * 16.0f + x, 2) + Math.Pow(position.y * 16.0f + y, 2) + Math.Pow(position.z * 16.0f + z, 2);
							if (distToCenter < radius)
								chunkData[x + z * Width + y * Width * Width] = 1;
							else if (distToCenter > radius + elevation)
								chunkData[x + z * Width + y * Width * Width] = 0;
							else
							{
								if (tab[x + z * Width + y * Width * Width] * ( 1 - (distToCenter - radius) / (elevation)) > 0.1f)
									chunkData[x + z * Width + y * Width * Width] = 3;
								else
									chunkData[x + z * Width + y * Width * Width] = 0;
							}
							_checkSum += chunkData[x + z * Width + y * Width * Width];
						}
					}
				}
			}
		}
	}
}
