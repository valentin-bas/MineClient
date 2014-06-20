using ICSharpCode.SharpZipLib.Zip.Compression;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MineServer
{
	public class Protocol
	{
		private int _state;
		public int State
		{
			get { return _state; }
			set { _state = value; }
		}
		private Client _client;
		private World _world;
		private Player _player;

		public Protocol(Client client, World world)
		{
			_state = 0;
			_client = client;
			_world = world;
		}

		~Protocol()
		{
		}

		//------------------------------------------------------------

		public int HandlePacket(BytesBuffer stream)
		{
			int totalLength = 0;
			int length;

			stream.Position = 0;
			try
			{
				while (stream.Position < stream.Length)
				{
					length = stream.ReadVarInt();
					if (length > stream.Length - stream.Position)
						return totalLength;
					BytesBuffer packetToHandle = new BytesBuffer();
					packetToHandle.Write(stream.GetBuffer(), (int)stream.Position, length);
					packetToHandle.Position = 0;
					int type = packetToHandle.ReadVarInt();
					if (_state == 0)
					{
							 if (type == 0x00)	HandleHandshake(packetToHandle);
					}
					else if (_state == 1)
					{
						     if (type == 0x00)	HandleStatusRequest(packetToHandle);
						else if (type == 0x01)	HandlePing(packetToHandle);
					}
					else if (_state == 2)
					{
							 if (type == 0x00)	HandleLoginStart(packetToHandle);
					}
					else if (_state == 3)
					{
							 if (type == 0x00)	HandleKeepAlive(packetToHandle);
						else if (type == 0x01)	HandleChatMessage(packetToHandle);
						else if (type == 0x03)	HandlePlayer(packetToHandle);
						else if (type == 0x04)	HandlePlayerPosition(packetToHandle);
						else if (type == 0x05)	HandlePlayerLook(packetToHandle);
						else if (type == 0x06)	HandlePlayerPositionLook(packetToHandle);
						else if (type == 0x07)	HandlePlayerDigging(packetToHandle);
						else if (type == 0x08)	HandlePlayerBlockPlacement(packetToHandle);
						else if (type == 0x16)	HandleClientStatus(packetToHandle);
						else if (type == 0x17)	HandlePluginMsg(packetToHandle);
						else					Logger.Instance.Log("UnHandlePacket : " + type);
					}
					stream.Position += length;
					totalLength = (int)stream.Position;
				}
			}
			catch (System.ArgumentException e)
			{
				System.Diagnostics.Debug.WriteLine(e.ToString());
				return 0;
			}
			return totalLength;
		}

		// (_state == 0):

		private void HandleHandshake(BytesBuffer buffer)
		{
			Logger.Instance.Log("Protocol Version : " + buffer.ReadVarInt());
			int size = buffer.ReadVarInt();
			byte[] buf = new byte[size];
			buffer.Read(buf, 0, (int)size);
			string serverAddr = Encoding.UTF8.GetString(buf);
			Logger.Instance.Log("Server Addr : " + serverAddr);
			Logger.Instance.Log("Server Port : " + buffer.ReadShort());
			int state = buffer.ReadVarInt();
			Logger.Instance.Log("Next State : " + state);
			if (state > 0 && state < 3)
				_state = state;
		}

		// (_state == 1):

		private void HandleStatusRequest(BytesBuffer buffer)
		{
			Packetizer packet = new Packetizer(_client);

			packet.WriteVarInt(0x00);
			string response = "{\"version\":{\"name\":\"1.7.2\",\"protocol\":4},\"players\":{" +
								"\"max\":30,\"online\":" + _world.GetPlayerCount() + ",\"sample\":[]}," +
								"\"description\":{\"text\":\"GP Team\"}" +
				//"\"favicon\":\"data:image/png;base64,%s\"" +
								"}";
			packet.WriteUTF8String(response);
			packet.End();
			packet.Send();
		}

		private void HandlePing(BytesBuffer buffer)
		{
			Packetizer packet = new Packetizer(_client);

			packet.WriteVarInt(0x01);
			packet.WriteLong(buffer.ReadLong());
			packet.End();
			packet.Send();
		}

		// (_state == 2):

		private void HandleLoginStart(BytesBuffer buffer)
		{
			Player player = new Player(_client, this);

			_player = player;
			player.Name = buffer.ReadUTF8String();

			Packetizer packet = new Packetizer(_client);
			packet.WriteVarInt(0x02);
			packet.WriteUTF8String(player.UUID.ToString());
			packet.WriteUTF8String(player.Name);
			packet.End();
			packet.WriteVarInt(0x01);
			packet.WriteInt((int)player.ID);
			packet.WriteByte((byte)_world.WorldType);
			packet.WriteByte(0);
			packet.WriteByte(2);
			packet.WriteByte(30);
			packet.WriteUTF8String("default");
			packet.End();
			packet.WriteVarInt(0x05);
			packet.WriteInt(_world.SpawnPosX);
			packet.WriteInt(_world.SpawnPosY);
			packet.WriteInt(_world.SpawnPosZ);
			packet.End();
			packet.WriteVarInt(0x39);
			byte flag;
			if (_world.WorldType == World.EWorldType.SURVIVAL)
				flag = 0;
			else
				flag = 0x01 | 0x08 | 0x02 | 0x04;
			packet.WriteByte(flag);
			packet.WriteFloat(0.10f);
			packet.WriteFloat(0.10f);
			packet.End();
			packet.WriteVarInt(0x03);
			packet.WriteLong(225);
			packet.WriteLong(225);
			packet.End();

			packet.WriteVarInt(0x30);
			packet.WriteByte(0);
			packet.WriteShort(45);
			for (int i = 0; i < 90; ++i)
				packet.WriteByte(0xFF);
			packet.End();

			FillUpdateHealth(packet, player);

			FillPlayerPositionAndLook(packet, player);

			packet.WriteVarInt(0x1F);
			packet.WriteFloat(0.0f);
			packet.WriteShort(0);
			packet.WriteShort(0);
			packet.End();

			packet.Send();
			_client.SetPlayer(player);
			_world.AddPlayer(player);
			player.SendAllChunkInProximity(_world.GetAllChunkInProximity(new Vector3Int(_world.SpawnPosX, _world.SpawnPosY, _world.SpawnPosZ), _player));
			_state = 3;
		}

		// (_state == 3):

		private void HandleKeepAlive(BytesBuffer buffer)
		{
			//TODO
		}

		private void HandlePluginMsg(BytesBuffer buffer)
		{
			Packetizer packet = new Packetizer(_client);

			packet.WriteVarInt(0x3f);
			packet.WriteUTF8String("MC|Brand");
			packet.WriteShort(0);
			packet.End();
			packet.Send();
		}

		private void HandlePlayer(BytesBuffer buffer)
		{
			_player.OnGround = buffer.ReadBool();
		}

		private void HandlePlayerPosition(BytesBuffer buffer)
		{
			_player.PosX = buffer.ReadDouble();
			_player.PosYFeet = buffer.ReadDouble();
			_player.PosYHead = buffer.ReadDouble();
			_player.PosZ = buffer.ReadDouble();
			_player.OnGround = buffer.ReadBool();

			if (_player.CheckChangedChunk())
				_player.SendAllChunkInProximity(_world.GetAllChunkInProximity(new Vector3Int((int)_player.PosX, (int)_player.PosY, (int)_player.PosZ), _player));
		}

		private void HandlePlayerLook(BytesBuffer buffer)
		{
			_player.Yaw = buffer.ReadFloat();
			_player.Pitch = buffer.ReadFloat();
			_player.OnGround = buffer.ReadBool();
		}

		private void HandlePlayerPositionLook(BytesBuffer buffer)
		{
			_player.PosX = buffer.ReadDouble();
			_player.PosYFeet = buffer.ReadDouble();
			_player.PosYHead = buffer.ReadDouble();
			_player.PosZ = buffer.ReadDouble();
			_player.Yaw = buffer.ReadFloat();
			_player.Pitch = buffer.ReadFloat();
			_player.OnGround = buffer.ReadBool();
			if (_player.CheckChangedChunk())
				_player.SendAllChunkInProximity(_world.GetAllChunkInProximity(new Vector3Int((int)_player.PosX, (int)_player.PosY, (int)_player.PosZ), _player));
		}

		private void HandlePlayerDigging(BytesBuffer buffer)
		{
			byte status = buffer.ReadByte();
			int x = buffer.ReadInt();
			byte y = buffer.ReadByte();
			int z = buffer.ReadInt();
			byte face = buffer.ReadByte();
			if ((status == 2 && _world.WorldType == World.EWorldType.SURVIVAL) ||
				(status == 0 && _world.WorldType == World.EWorldType.CREATIVE))
				_world.UpdateBlockAt(x, y, z, 0);
		}

		private void HandlePlayerBlockPlacement(BytesBuffer buffer)
		{
			int x = buffer.ReadInt();
			byte y = buffer.ReadByte();
			int z = buffer.ReadInt();
			byte dir = buffer.ReadByte();
			short id = buffer.ReadSlot();
			byte cursorX = buffer.ReadByte();
			byte cursorY = buffer.ReadByte();
			byte cursorZ = buffer.ReadByte();
			if (dir == 0)
				y--;
			else if (dir == 1)
				y++;
			else if (dir == 2)
				z--;
			else if (dir == 3)
				z++;
			else if (dir == 4)
				x--;
			else if (dir == 5)
				x++;
			if (x == -1 && z == -1 && y == 255 && dir == 255)
				return;
			if (id == 326)
				id = 9;
			_world.UpdateBlockAt(x, y, z, (byte)id);
		}

		private void HandleChatMessage(BytesBuffer buffer)
		{
			string tmp = buffer.ReadUTF8String();
			tmp = tmp.Replace('\\', ' ');
			_world.BroadcastMessage(tmp, _player);
		}

		private void HandleClientStatus(BytesBuffer buffer)
		{
			byte ActionId = buffer.ReadByte();

			if (ActionId == 0)
				_world.RespawnPlayer(_player);
		}

		//---------------------------------------------

		public void FillKeepAlive(Packetizer packet)
		{
			Random rand = new Random();
			packet.WriteVarInt(0x00);
			packet.WriteInt(rand.Next());
			packet.End();
		}

		public void FillChunk(Packetizer packet, Chunk chunck)
		{
			packet.WriteVarInt(0x21);
			packet.WriteInt(chunck.position.x);
			packet.WriteInt(chunck.position.y);
			packet.WriteInt(chunck.position.z);
			packet.WriteBool(true);
			packet.WriteLEUShort(0xffff);
			packet.WriteShort(0);
			int length;
			byte[] data = chunck.GetCompressData(out length);
			packet.WriteInt(length);
			packet.Write(data, 0, length);
			packet.End();
		}

		public void FillUnloadChunk(Packetizer packet, Vector3Int pos)
		{
			packet.WriteVarInt(0x21);
			packet.WriteInt(pos.x);
			packet.WriteInt(pos.y);
			packet.WriteInt(pos.z);
			packet.WriteBool(true);
			packet.WriteLEUShort(0x00);
			packet.WriteShort(0);
			packet.WriteInt(0);
			packet.End();
		}

		public void FillSpawnPlayer(Packetizer packet, Player player)
		{
			packet.WriteVarInt(0x0c);
			packet.WriteVarInt((int)player.ID);
			packet.WriteUTF8String("" + player.UUID);
			packet.WriteUTF8String(player.Name);
			packet.WriteIntFixedPoint(player.PosX);
			packet.WriteIntFixedPoint(player.PosYFeet);
			packet.WriteIntFixedPoint(player.PosZ);
			packet.WriteByteAngle(player.Yaw);
			packet.WriteByteAngle(player.Pitch);
			packet.WriteShort(0);
			packet.WriteByte(0x66);
			packet.WriteFloat(100.0f);
			packet.WriteByte(0x7f);
			packet.End();
		}

		/*public void FillSpawnObject(Packetizer packet, FallingBlock block)
		{
			packet.WriteVarInt(0x0e);
			packet.WriteVarInt((int)block.ID);
			packet.WriteByte(70);
			packet.WriteIntFixedPoint(block.PosX);
			packet.WriteIntFixedPoint(block.PosYFeet);
			packet.WriteIntFixedPoint(block.PosZ);
			packet.WriteByteAngle(block.Pitch);
			packet.WriteByteAngle(block.Yaw);
			packet.WriteInt(block.IdBlock);
			packet.WriteShort(0);
			packet.WriteShort(0);
			packet.WriteShort(0);
			packet.End();
		}*/

		public void FillEntityRelativeMove(Packetizer packet, Entity entity)
		{
			packet.WriteVarInt(0x15);
			packet.WriteInt((int)entity.ID);
			packet.WriteByteFixedPoint(entity.RelX);
			packet.WriteByteFixedPoint(entity.RelY);
			packet.WriteByteFixedPoint(entity.RelZ);
			packet.End();
		}

		public void FillEntityLook(Packetizer packet, Entity entity)
		{
			packet.WriteVarInt(0x16);
			packet.WriteInt((int)entity.ID);
			packet.WriteByteAngle(entity.Yaw);
			packet.WriteByteAngle(entity.Pitch);
			packet.End();
		}

		public void FillEntityRelativeMoveLook(Packetizer packet, Entity entity)
		{
			packet.WriteVarInt(0x17);
			packet.WriteInt((int)entity.ID);
			packet.WriteByteFixedPoint(entity.RelX);
			packet.WriteByteFixedPoint(entity.RelY);
			packet.WriteByteFixedPoint(entity.RelZ);
			packet.WriteByteAngle(entity.Yaw);
			packet.WriteByteAngle(entity.Pitch);
			packet.End();
		}

		public void FillEntityTeleport(Packetizer packet, Entity entity)
		{
			packet.WriteVarInt(0x18);
			packet.WriteInt((int)entity.ID);
			packet.WriteIntFixedPoint(entity.PosX);
			packet.WriteIntFixedPoint(entity.PosYFeet);
			packet.WriteIntFixedPoint(entity.PosZ);
			packet.WriteByteAngle(entity.Yaw);
			packet.WriteByteAngle(entity.Pitch);
			packet.End();
		}

		public void FillEntityHeadLook(Packetizer packet, Entity entity)
		{
			packet.WriteVarInt(0x19);
			packet.WriteInt((int)entity.ID);
			packet.WriteByteAngle(entity.Yaw);
			packet.End();
		}

		public void FillBlockChange(Packetizer packet, int x, int y, int z, byte id, byte meta = 0)
		{
			packet.WriteVarInt(0x23);
			packet.WriteInt(x);
			packet.WriteInt(y);
			packet.WriteInt(z);
			packet.WriteVarInt(id);
			packet.WriteByte(meta);
			packet.End();
		}

		public void FillAnimation(Packetizer packet, int id, byte animationType)
		{
			packet.WriteVarInt(0x0B);
			packet.WriteVarInt(id);
			packet.WriteByte(animationType);
			packet.End();
		}

		public void FillEntityMetadata(Packetizer packet, Player from)
		{
			byte flag = 0;

			if (from.isCrouched)
				flag |= 0x02;
			if (from.isSrinting)
				flag |= 0x08;
			/* OnFire 0x01
			 * IsCrouched 0x02
			 * IsSprinting 0x08
			 * IsRclKing 0x10
			 * IsInvisible 0x20
			 */
			packet.WriteVarInt(0x1c);
			packet.WriteInt((int)from.ID);

			packet.WriteByte(0);
			packet.WriteByte(flag);

			/* Additionnal Information Can be written here */

			packet.WriteByte(0x7f);
			packet.End();
		}

		public void FillDestroyEntity(Packetizer packet, Entity entity)
		{
			packet.WriteVarInt(0x13);
			packet.WriteByte(1);
			packet.WriteInt((int)entity.ID);
			packet.End();
		}

		public void FillChatMessage(Packetizer packet, Player player, string message)
		{
			packet.WriteVarInt(0x02);
			string tmp = "{\"text\":\"<" + player.Name + "> " + message + "\"}";
			packet.WriteUTF8String(tmp);
			packet.End();
		}

		public void FillUpdateHealth(Packetizer packet, Player player)
		{
			packet.WriteVarInt(0x06);
			packet.WriteFloat(player.Health);
			packet.WriteShort(20);
			packet.WriteFloat(5.0f);
			packet.End();
		}

		public void FillRespawn(Packetizer packet)
		{
			packet.WriteVarInt(0x07);
			packet.WriteInt(0);
			packet.WriteByte(0);
			packet.WriteByte((byte)_world.WorldType);
			packet.WriteUTF8String("default");
			packet.End();
		}

		public void FillPlayerPositionAndLook(Packetizer packet, Player player)
		{
			packet.WriteVarInt(0x08);
			packet.WriteDouble((double)_world.SpawnPosX);
			packet.WriteDouble((double)_world.SpawnPosY);
			packet.WriteDouble((double)_world.SpawnPosZ);
			packet.WriteFloat(0.0f);
			packet.WriteFloat(0.0f);
			packet.WriteBool(true);
			packet.End();
		}

		public void FillExplosion(Packetizer packet, Explosion explo)
		{
			packet.WriteVarInt(0x27);
			packet.WriteFloat(explo.PosX);
			packet.WriteFloat(explo.PosY);
			packet.WriteFloat(explo.PosZ);
			packet.WriteFloat(explo.Radius);
			packet.WriteInt(explo.Records.Count / 3);
			foreach (byte it in explo.Records)
				packet.WriteByte(it);
			packet.WriteFloat(0.0f);
			packet.WriteFloat(0.0f);
			packet.WriteFloat(0.0f);
			packet.End();
		}
	}
}
