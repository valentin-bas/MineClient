using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MineServer
{
	public class BytesBuffer : MemoryStream
	{
		public BytesBuffer()
			: base()
		{
		}

		public int ReadVarInt()
		{
			int shift = 0;
			int result = 0;
			const int sizeBites = 32;
			int byteValue;
			int i = 0;
			while (i < base.Length)
			{
				byteValue = base.ReadByte();
				int tmp = byteValue & 0x7f;
				result |= tmp << shift;
				if (shift > sizeBites)
				{
					throw new System.ArgumentOutOfRangeException("bytes", "Byte array is too large.");
				}
				if ((byteValue & 0x80) != 0x80)
				{
					return result;
				}
				shift += 7;
				i++;
			}
			throw new System.ArgumentException("Cannot decode varint from byte array.", "bytes");
		}

		public long ReadLong()
		{
			byte[] buf = new byte[sizeof(long)];
			base.Read(buf, 0, sizeof(long));
			return System.Net.IPAddress.NetworkToHostOrder(BitConverter.ToInt64(buf, 0));
		}

		public short ReadShort()
		{
			byte[] buf = new byte[sizeof(short)];
			base.Read(buf, 0, sizeof(short));
			return System.Net.IPAddress.NetworkToHostOrder(BitConverter.ToInt16(buf, 0));
		}

		public int ReadInt()
		{
			byte[] buf = new byte[sizeof(int)];
			base.Read(buf, 0, sizeof(int));
			return System.Net.IPAddress.NetworkToHostOrder(BitConverter.ToInt32(buf, 0));
		}

		public uint ReadUInt()
		{
			byte[] buf = new byte[sizeof(uint)];
			base.Read(buf, 0, sizeof(uint));
			return (uint)System.Net.IPAddress.NetworkToHostOrder(BitConverter.ToInt32(buf, 0));
		}

		public float ReadFloat()
		{
			byte[] buf = new byte[sizeof(float)];
			base.Read(buf, 0, sizeof(float));
			if (BitConverter.IsLittleEndian)
				Array.Reverse(buf);
			return BitConverter.ToSingle(buf, 0);
		}

		public double ReadDouble()
		{
			byte[] buf = new byte[sizeof(double)];
			base.Read(buf, 0, sizeof(double));
			if (BitConverter.IsLittleEndian)
				Array.Reverse(buf);
			return BitConverter.ToDouble(buf, 0);
		}

		public bool ReadBool()
		{
			byte[] buf = new byte[sizeof(bool)];
			base.Read(buf, 0, sizeof(bool));
			if (BitConverter.IsLittleEndian)
				Array.Reverse(buf);
			return BitConverter.ToBoolean(buf, 0);
		}

		public new byte ReadByte() 
		{
			byte[] buf = new byte[sizeof(byte)];
			base.Read(buf, 0, sizeof(byte));
			if (BitConverter.IsLittleEndian)
				Array.Reverse(buf);
			return buf[0];
		}

		public short ReadSlot()
		{
			short id = ReadShort();
			if (id == -1)
				return id;
			byte itemCount = ReadByte();
			short itemDamage = ReadShort();
			short NBTLength = ReadShort();
			if (NBTLength == -1)
				return id;
			byte[] buf = new byte[NBTLength];
			Read(buf, 0, NBTLength);
			return id;
		}

		public string ReadUTF8String()
		{
			int size = ReadVarInt();
			byte[] bytes = new byte[size];

			base.Read(bytes, 0, (int)size);
			return Encoding.UTF8.GetString(bytes);
		}

		public void WriteUShort(ushort value)
		{
			byte[] bytes = BitConverter.GetBytes(System.Net.IPAddress.HostToNetworkOrder(value));
			base.Write(bytes, 0, bytes.Length);
		}

		public void WriteLEUShort(ushort value)
		{
			byte[] bytes = BitConverter.GetBytes(value);
			base.Write(bytes, 0, bytes.Length);
		}

		public void WriteShort(short value)
		{
			byte[] bytes = BitConverter.GetBytes(System.Net.IPAddress.HostToNetworkOrder(value));
			base.Write(bytes, 0, bytes.Length);
		}

		public void WriteInt(int value)
		{
			byte[] bytes = BitConverter.GetBytes(System.Net.IPAddress.HostToNetworkOrder(value));
			base.Write(bytes, 0, bytes.Length);
		}

		public void WriteLong(long value)
		{
			byte[] bytes = BitConverter.GetBytes(System.Net.IPAddress.HostToNetworkOrder(value));
			base.Write(bytes, 0, bytes.Length);
		}

		public void WriteDouble(double value)
		{
			byte[] bytes = BitConverter.GetBytes(value);
			if (BitConverter.IsLittleEndian)
				Array.Reverse(bytes);
			base.Write(bytes, 0, bytes.Length);
		}

		public void WriteFloat(float value)
		{
			byte[] bytes = BitConverter.GetBytes(value);
			if (BitConverter.IsLittleEndian)
				Array.Reverse(bytes);
			base.Write(bytes, 0, bytes.Length);
		}

		public void WriteBool(bool value)
		{
			byte[] bytes = BitConverter.GetBytes(value);
			if (BitConverter.IsLittleEndian)
				Array.Reverse(bytes);
			base.Write(bytes, 0, bytes.Length);
		}

		public void WriteVarInt(int value)
		{
			byte[] bytes = new byte[5];
			int idx = 0;

			do
			{
				bytes[idx] = (byte)((value & 0x7f) | ((value > 0x7f) ? 0x80 : 0x00));
				value = value >> 7;
				idx++;
			} while (value > 0);
			base.Write(bytes, 0, idx);
		}

		public void WriteUTF8String(string value)
		{
			WriteVarInt(value.Length);
			base.Write(Encoding.UTF8.GetBytes(value), 0, Encoding.UTF8.GetByteCount(value));
		}

		public void WriteIntFixedPoint(double value)
		{
			int abs_int = (int)(value * 32.0d);
			WriteInt(abs_int);
		}

		public void WriteByteFixedPoint(double value)
		{
			int abs_int = (int)(value * 32.0d);
			WriteByte((byte)abs_int);
		}

		public void WriteByteAngle(double value)
		{
			byte tmp = (byte)(255 * ((value % 360.0d) / 360.0d));
			WriteByte(tmp);
		}
	}
}