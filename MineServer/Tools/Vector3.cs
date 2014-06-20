using System;
using System.Collections.Generic;
using System.Linq;

namespace MineServer
{
	public class Vector3 : IEquatable<Vector3>
	{
		int _x;
		byte _y;
		int _z;

		public int x
		{
			get { return _x; }
			set { _x = value; }
		}

		public byte y
		{
			get { return _y; }
			set { _y = value; }
		}

		public int z
		{
			get { return _z; }
			set { _z = value; }
		}

		public Vector3(int x, byte y, int z)
		{
			_x = x;
			_y = y;
			_z = z;
		}

		public double SquareDist(Vector3 other)
		{
			return Math.Pow(other._x - _x, 2) + Math.Pow(other._y - _y, 2) + Math.Pow(other._z - _z, 2);
		}

		public double Distance(Vector3 other)
		{
			double dist = Math.Pow(other._x - _x, 2) + Math.Pow(other._y - _y, 2) + Math.Pow(other._z - _z, 2);
			return Math.Sqrt(dist);
		}

		public static bool operator ==(Vector3 left, Vector3 right)
		{
			if (left._x == right._x && left._y == right._y && left._z == right._z)
				return true;
			return false;
		}

		public static bool operator !=(Vector3 left, Vector3 right)
		{
			if (left._x != right._x || left._y != right._y || left._z != right._z)
				return true;
			return false;
		}

		public bool Equals(Vector3 obj)
		{
			return obj == this;
		}

		public override bool Equals(Object o)
		{
			Vector3 obj = o as Vector3;
			if (obj == null)
				return false;
			return obj == this;
		}

		public override int GetHashCode()
		{
			int hash = 23;
			hash = hash * 31 + _x;
			hash = hash * 31 + _y;
			hash = hash * 31 + _z;
			hash = hash * 31 + 8;
			return hash;
		}
	}
}
