using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace MineServer
{
	public class Entity
	{
		public enum EAnimation : byte
		{
			SWING,
			DAMAGE,
			LEAVE_BED,
			EAT,
			CRITICAL,
			MAGIC
		};

		public event Action<Double> NotifyXChange;
		public event Action<Double> NotifyYChange;
		public event Action<Double> NotifyZChange;

		private double _oldPosX;
		private double _oldPosYFeet;
		private double _oldPosYHead;
		private double _oldPosZ;
		public double RelX
		{
			get { return _posX - _oldPosX; }
		}
		public double RelY
		{
			get { return _posYFeet - _oldPosYFeet; }
		}
		public double RelZ
		{
			get { return _posZ - _oldPosZ; }
		}
		
		private double _posX;
		private double _posYFeet;
		private double _posYHead;
		
		private double _posZ;
		private float _Yaw;
		private float _Pitch;
		private bool _OnGround;

		public double PosX
		{
			get { lock (this) { return _posX; } }
			set { lock (this) { _posX = value; } if (NotifyXChange != null) NotifyXChange(_posX); }
		}
		public double PosY
		{
			get { lock (this) { return _posYFeet; } }
			set { lock (this) { _posYFeet = value; } if (NotifyYChange != null) NotifyYChange(_posYFeet); }
		}
		public double PosYFeet
		{
			get { lock (this) { return _posYFeet; } }
			set { lock (this) { _posYFeet = value; } if (NotifyYChange != null) NotifyYChange(_posYFeet); }
		}
		public double PosYHead
		{
			get { lock (this) { return _posYHead; } }
			set { lock (this) { _posYHead = value; } }
		}
		public double PosZ
		{
			get { lock (this) { return _posZ; } }
			set { lock (this) { _posZ = value; } if (NotifyZChange != null) NotifyZChange(_posZ); }
		}
		public float Yaw
		{
			get { lock (this) { return _Yaw; } }
			set { lock (this) { _Yaw = value; } }
		}
		public float Pitch
		{
			get { lock (this) { return _Pitch; } }
			set { lock (this) { _Pitch = value; } }
		}
		public bool OnGround
		{
			get { lock (this) { return _OnGround; } }
			set { lock (this) { _OnGround = value; } }
		}
		private uint _ID;
		public uint ID
		{
			get { return _ID; }
		}
		private static uint _nextID = 0;
		private static object _LOCK = new object();

		public Entity()
		{
			lock (_LOCK)
			{
				_ID = _nextID;
				_nextID++;
			}
		}

		public void ResetOldPos()
		{
			_oldPosX = _posX;
			_oldPosYFeet = _posYFeet;
			_oldPosYHead = _posYHead;
			_oldPosZ = _posZ;
		}
	}
}
