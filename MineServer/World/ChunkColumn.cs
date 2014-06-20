using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ICSharpCode.SharpZipLib.Zip.Compression;
using System.Collections.Concurrent;

namespace MineServer
{
	/*
	public class ChunkColumn
	{
		public const int Width = 16;
		public const int Height = 256;
		public const int ChunkCount = 16;
		public const int NumBlocks = Width * Height * Width;
		public const int BiomeDataSize = Width * Width;

		public const int MetadataOffset = sizeof(byte) * NumBlocks;
		public const int BlockLightOffset = MetadataOffset + sizeof(byte) * (NumBlocks / 2);
		public const int SkyLightOffset = BlockLightOffset + sizeof(byte) * (NumBlocks / 2);
		public const int BiomeOffset = SkyLightOffset + sizeof(byte) * (NumBlocks / 2);

		public const int DataSize = BiomeOffset + BiomeDataSize;
		private byte[] chunkData = new byte[DataSize];
		private Chunk[] _chunks = new Chunk[ChunkCount];

		private Vector2Int _position;
		public Vector2Int Position
		{
			get { return _position; }
		}
		private World _world;

		public ChunkColumn(Vector2Int position, World world)
		{
			_position = position;
			_world = world;
			for (int i = BlockLightOffset; i < BlockLightOffset + NumBlocks / 2; ++i)
				chunkData[i] = 0xff;
			for (int i = SkyLightOffset; i < SkyLightOffset + NumBlocks / 2; ++i)
				chunkData[i] = 0xff;
			for (int i = 0; i < ChunkCount; ++i)
			{
				_chunks[i] = new Chunk(i, this);
			}
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

		public void UpdateBlockAt(int x, byte y, int z, byte id)
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
				if ((id == 12 || id == 13) && y > 0 &&
					chunkData[relX + relZ + (y - 1) * 256] == 0)
				{
					_world.CreateFallingBlock(id, x, y, z);
					chunkData[relX + relZ + y * 256] = 0;
				}
				else if (id == 0 && y < 255 &&
						 (chunkData[relX + relZ + (y + 1) * 256] == 12 ||
						  chunkData[relX + relZ + (y + 1) * 256] == 13))
				{
					_world.CreateFallingBlock(chunkData[relX + relZ + (y + 1) * 256], x, y + 1, z);
					chunkData[relX + relZ + y * 256] = 0;
					chunkData[relX + relZ + (y + 1) * 256] = 0;
					for (int i = y + 2; i < 256; ++i)
					{
						byte type = chunkData[relX + relZ + i * 256];
						if (type != 12 && type != 13)
							break;
						_world.CreateFallingBlock(chunkData[relX + relZ + i * 256], x, i, z);
						chunkData[relX + relZ + i * 256] = 0;
					}
				}
				else if (id == 0 && chunkData[relX + relZ + y * 256] == 46)
				{
					Explosion explo = new Explosion();
					chunkData[relX + relZ + y * 256] = 0;
					explo.PosX = x + 0.5f;
					explo.PosY = y + 0.5f;
					explo.PosZ = z + 0.5f;
					explo.Radius = 5.0f;


					float tmp;
					ChunkColumn currentChunk = this;
					for (int i = -(int)explo.Radius; i <= explo.Radius; ++i)
					{
						for (int j = -(int)explo.Radius; j <= explo.Radius; ++j)
						{
							if (y + j >= 0 && y + j < 256)
							{
								for (int k = -(int)explo.Radius; k <= explo.Radius; ++k)
								{
									tmp = i * i + j * j + k * k;
									if (tmp <= explo.Radius * explo.Radius)
									{
										explo.Records.Add((byte)(i - 1));
										explo.Records.Add((byte)j);
										explo.Records.Add((byte)k);

										int tmpX = (x + i) < 0 ? 16 + (x + i) % 16 : (x + i) % 16;
										if (tmpX == 16)
											tmpX = 0;
										int tmpZ = (z + k) < 0 ? 16 + (z + k) % 16 : (z + k) % 16;
										if (tmpZ == 16)
											tmpZ = 0;
										tmpZ *= 16;

										int toto = (x + i);
										int tata = (z + k);
										currentChunk = _world.GetChunkColumn(new Vector2Int(x + i, z + k));
										if (currentChunk.chunkData[tmpX + tmpZ + (y + j) * 256] == 46)
											currentChunk.UpdateBlockAt(x + i, (byte)(y + j), z + k, 0);
										else
											currentChunk.chunkData[tmpX + tmpZ + (y + j) * 256] = 0;
									}
								}
							}
						}
					}

					_world.BroadcastExplosion(explo);
				}
				else
					chunkData[relX + relZ + y * 256] = id;
			}
		}

		public void HardSetBlockAt(int x, byte y, int z, byte id)
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
				chunkData[relX + relZ + y * 256] = id;
			}
		}

		public byte GetBlockTypeAt(int x, byte y, int z)
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
				return chunkData[relX + relZ + y * 256];
			}
		}

		public void HardSetMetadataAt(int x, byte y, int z, byte data)
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

				byte mask;
				if (relX % 2 != 0)
				{
					mask = 0x0f;
					data <<= 4;
					data &= 0xf0;
				}
				else
					mask = 0xf0;

				chunkData[MetadataOffset + (relX + relZ + y * 256) / 2] &= mask;
				chunkData[MetadataOffset + (relX + relZ + y * 256) / 2] |= data;
			}
		}

		public byte GetMetadataAt(int x, byte y, int z)
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
				byte byteTmp = chunkData[MetadataOffset + (relX + relZ + y * 256) / 2];

				if (relX % 2 != 0)
				{
					byteTmp >>= 4;
					byteTmp &= 0x0f;
				}
				else
					byteTmp &= 0x0f;
				return byteTmp;
			}
		}

		public void GenerateNoiseWorld()
		{
			lock (this)
			{
				float[] tab = new float[16 * 16];
				for (int i = 0; i < 16; ++i)
				{
					for (int j = 0; j < 16; ++j)
					{
						tab[i * 16 + j] = Noise.Noise.GetOctaveNoise(((Position.x) * 16.0f + j) / 128.0f, ((Position.y) * 16.0f + i) / 128.0f, 0, 8) * 30.0f + 50.0f;
					}
				}
				for (int y = 0; y < 256; ++y)
				{
					for (int z = 0; z < 16; ++z)
					{
						for (int x = 0; x < 16; ++x)
						{
							if (y > tab[z * 16 + x])
							{
								if (y < 64)
									chunkData[x + z * 16 + y * 256] = (byte)(8);
								else
									chunkData[x + z * 16 + y * 256] = (byte)(0);
							}
							else if (y > tab[z * 16 + x] - 1)
							{
								if (y < 65)
									chunkData[x + z * 16 + y * 256] = (byte)(12);
								else
									chunkData[x + z * 16 + y * 256] = (byte)(2);
							}
							else if (y > tab[z * 16 + x] - 5)
								chunkData[x + z * 16 + y * 256] = (byte)(3);
							else
								chunkData[x + z * 16 + y * 256] = (byte)(1);
							if (y < 2)
								chunkData[x + z * 16 + y * 256] = (byte)(7);
						}
					}
				}
			}
		}
	}
	*/
}
