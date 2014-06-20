#pragma once

#include "Frustum.h"

namespace vox
{
	class Chunk;
	class ChunkColumn
	{
	public:
		static const int CHUNK_COUNT = 16;

		ChunkColumn(void);
		~ChunkColumn(void);

		bool	Init(int PosX, int PosZ, unsigned char* data);
		bool	InitGL(void);
		bool	Render(GLint attrCoord3D, GLint attrVertexColor, GLint attrVertexNormal,
					   GLint attrUniformModel, Frustum& frustum, GLint attrTexture);
		void	Destroy(void);
		bool	getBlockAt(int chunkKey, int x, int y, int z, int id);
		int		getBlockAt(int x, int y, int z);
	private:
		Chunk*	_chunks;
		int		_posX;
		int		_posZ;
	};

}