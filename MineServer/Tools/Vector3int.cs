using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MineServer
{
	public class Vector3Int : IEquatable<Vector3Int>
	{
		public int x { get; set; }
		public int y { get; set; }
		public int z { get; set; }

		public Vector3Int(int x = 0, int y = 0, int z = 0)
		{
			this.x = x;
			this.y = y;
			this.z = z;
		}

		public double Distance(Vector3Int other)
		{
			double dist = Math.Pow(other.x - x, 2) + Math.Pow(other.y - y, 2) + Math.Pow(other.z - z, 2);
			return Math.Abs(Math.Sqrt(dist));
		}

		public static bool operator ==(Vector3Int left, Vector3Int right)
		{
			if (left.x == right.x && left.y == right.y && left.z == right.z)
				return true;
			return false;
		}
		public static bool operator !=(Vector3Int left, Vector3Int right)
		{
			if (left.x != right.x || left.y != right.y || left.z != right.z)
				return true;
			return false;
		}

		public bool Equals(Vector3Int obj)
		{
			return obj == this;
		}

		public override bool Equals(Object o)
		{
			Vector3Int obj = o as Vector3Int;
			if (obj == null)
				return false;
			return obj == this;
		}

		public override int GetHashCode()
		{
			return 1 / 2 * (x + y + z) * (x + y  + z + 1) + y + z;
		}
	}
}
