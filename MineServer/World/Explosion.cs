using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MineServer
{
	public class Explosion
	{
		public float PosX { get; set; }
		public float PosY { get; set; }
		public float PosZ { get; set; }
		public float Radius { get; set; }
		public List<byte> Records { get; set; }

		public Explosion()
		{
			PosX = 0.0f;
			PosY = 0.0f;
			PosZ = 0.0f;
			Radius = 0.0f;
			Records = new List<byte>();
		}
	}
}
