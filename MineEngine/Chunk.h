#ifndef	__CHUNK_H__
#define	__CHUNK_H__

#include "Cube.h"

namespace	vox
{
	class	Frustum;
	class	Chunk
	{
		public:
			static const int CHUNK_SIZE = 16;
			static const int CHUNK_SCALE = 1;
		private:
			bool	__destroy(void);
					Chunk(Chunk const& right);
					Chunk&	operator=(const Chunk& right);

			unsigned char*	blocks;
			int				_x;
			int				_y;
			int				_z;
			int				_nbTriangle;
			GLuint*			_indicesArray;
			GLfloat*		_uvsArray;
			GLuint			vboUV;
			GLuint			vboIdx;
			
			static	GLuint	vboVertex;
			static	GLuint	vboNormal;
			static	GLuint	textures;
		protected:

		public:
					Chunk(int x = 0, int y = 0, int z = 0);
			virtual	~Chunk(void);
			bool	init(int x, int y, int z, unsigned char* data);
			bool	initGL(void);
			bool	update(float dt);
			bool	Render(GLint attrCoord3D, GLint attrVertexColor, GLint attrVertexNormal,
						   GLint attrUniformModel, Frustum& frustum, GLint attrTexture);
			bool	createMesh(void);
			void	bindMesh(void);
			bool	createCube(GLuint* indice, GLuint IArray[], int i, int x, int y, int z);
			void	copyIndice(GLuint* indice, GLuint IArray[], int i, int offset);
			bool	blockIsActive(int x, int y, int z);
			int		getPosX(void);
			int		getPosY(void);
			int		getPosZ(void);
			void	getPosition(glm::vec3& res);
			void	getPosition(Vector3Int& res);
			void	setBlockActive(int x, int y, int z, unsigned int id);
			int		GetBlockID(int x, int y, int z);
			bool	UpdateBlockAt(int x, int y, int z, int id);

			static bool	initVertex(void);

	};
}

#endif
