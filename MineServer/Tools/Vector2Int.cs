using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MineServer
{
	public class Vector2Int : IEquatable<Vector2Int>
	{
		public int x { get; set;}
		public int y { get; set;}

		public Vector2Int(int x = 0, int y = 0)
		{
			this.x = x;
			this.y = y;
		}

		public double Distance(Vector2Int other)
		{
			double dist = Math.Pow(other.x - x, 2) + Math.Pow(other.y - y, 2);
			return Math.Abs(Math.Sqrt(dist));
		}

		public static bool operator ==(Vector2Int left, Vector2Int right)
		{
			if (left.x == right.x && left.y == right.y)
				return true;
			return false;
		}
		public static bool operator !=(Vector2Int left, Vector2Int right)
		{
			if (left.x != right.x || left.y != right.y)
				return true;
			return false;
		}

		public bool Equals(Vector2Int obj)
		{
			return obj == this;
		}

		public override bool Equals(Object o)
		{
			Vector2Int obj = o as Vector2Int;
			if (obj == null)
				return false;
			return obj == this;
		}

		public override int GetHashCode()
		{
			return 1 / 2 * (x + y) * (x + y + 1) + y;
		}
	}
}
