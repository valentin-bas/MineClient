using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MineServer
{
	/*
	public class FallingBlock : Entity
	{
		private byte _idBlock;
		public byte IdBlock
		{
			get { return _idBlock; }
		}
		private ChunkColumn _column;
		private float _speed;
		public bool ToDelete { get; set; }

		public FallingBlock(byte idBlock, ChunkColumn column)
		{
			_idBlock = idBlock;
			_column = column;
			_speed = 0.0f;
			ToDelete = false;
		}

		public void Update(float dt)
		{
			_speed += -0.96f * dt;
			PosYFeet += _speed;
			int floorX = (int)Math.Floor(PosX);
			byte floorY = (byte)Math.Floor(PosYFeet);
			int floorZ = (int)Math.Floor(PosZ);
			if (PosYFeet < 0.0f)
				ToDelete = true;
			else if (_column.GetBlockTypeAt(floorX, floorY, floorZ) != 0)
			{
				int y = (int)floorY + 1;
				for (int i = y; i < 256; ++i)
				{
					byte type = _column.GetBlockTypeAt(floorX, (byte)i, floorZ);
					if (type == 0)
					{
						y = i;
						break;
					}
				}
				_column.HardSetBlockAt(floorX, (byte)y, floorZ, IdBlock);
				PosX = floorX;
				PosYFeet = y;
				PosZ = floorZ;
				ToDelete = true;
			}
		}
	}
	 */
}
