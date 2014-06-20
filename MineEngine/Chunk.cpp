#include "stdafx.h"
#include "Chunk.h"
#include "Image_Utils.h"
#include <iostream>
#include "Frustum.h"
#include "Vector3Int.h"

GLuint	vox::Chunk::vboVertex;
GLuint	vox::Chunk::vboNormal;
GLuint	vox::Chunk::textures;

vox::Chunk::Chunk(int x, int y, int z) :
	_x(x), _y(y), _z(z),
	_indicesArray(nullptr), _uvsArray(nullptr)
{
}

vox::Chunk::Chunk(vox::Chunk const& right)
{
	(void)right;
}

vox::Chunk::~Chunk(void)
{
	this->__destroy();
}

bool
vox::Chunk::initVertex(void)
{
	Chunk::textures = loadDDS("blockTex.dds");
	if (Chunk::textures <= 0)
		return false;

	GLfloat	cArray[24] = {
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f
	};
	
	GLfloat*	CubeArray = new (std::nothrow) GLfloat[72 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
	if (CubeArray == nullptr)
		return false;
	int	rest;
	int	i = 0;
	for (int x = 0 ; x < CHUNK_SIZE ; ++x)
	{
		for (int y = 0 ; y < CHUNK_SIZE ; ++y)
		{
			for (int z = 0 ; z < CHUNK_SIZE ; ++z)
			{
				for (int j = 0; j < 72; ++j)
				{
					rest = j % 3;
					if (rest == 0)
						CubeArray[i * 72 + j] = cArray[j % 24] + x * 2;
					else if (rest == 1)
						CubeArray[i * 72 + j] = cArray[j % 24] + y * 2;
					else
						CubeArray[i * 72 + j] = cArray[j % 24] + z * 2;
				}
				++i;
			}
		}
	}

	glGenBuffers(1, &vboVertex);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 72 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, CubeArray, GL_STATIC_DRAW);

	
	GLfloat	coArrayNormal[72] = {
		-1.0f, 0.0f, 0.0f,	//0
		-1.0f, 0.0f, 0.0f,	//1
		-1.0f, 0.0f, 0.0f,	//2
		-1.0f, 0.0f, 0.0f,	//3

		1.0f, 0.0f, 0.0f,	//4
		1.0f, 0.0f, 0.0f,	//5
		1.0f, 0.0f,	0.0f,	//6
		1.0f, 0.0f,	0.0f,	//7
		
		0.0f, 1.0f, 0.0f,	//8
		0.0f, -1.0f, 0.0f,	//9
		0.0f, 1.0f,	0.0f,	//10
		0.0f, -1.0f, 0.0f,	//11
		
		0.0f, 1.0f, 0.0f,	//12
		0.0f, -1.0f, 0.0f,	//13
		0.0f, 1.0f, 0.0f,	//14
		0.0f, -1.0f, 0.0f,	//15
		
		0.0f, 0.0f, -1.0f,	//16
		0.0f, 0.0f, -1.0f,	//17
		0.0f, 0.0f,	1.0f,	//18
		0.0f, 0.0f,	1.0f,	//19
		
		0.0f, 0.0f, 1.0f,	//20
		0.0f, 0.0f, 1.0f,	//21
		0.0f, 0.0f,	-1.0f,	//22
		0.0f, 0.0f,	-1.0f	//23
		
	};
	i = 0;
	for (int x = 0 ; x < CHUNK_SIZE ; ++x)
	{
		for (int y = 0 ; y < CHUNK_SIZE ; ++y)
		{
			for (int z = 0 ; z < CHUNK_SIZE ; ++z)
			{
				for (int j = 0; j < 72; ++j)
				{
					CubeArray[i * 72 + j] = coArrayNormal[j];
				}
				++i;
			}
		}
	}
	glGenBuffers(1, &vboNormal);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 72 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, CubeArray, GL_STATIC_DRAW);
	delete[] CubeArray;
	return true;
}

bool
vox::Chunk::init(int x, int y, int z, unsigned char* data)
{
	_x = x;
	_y = y;
	_z = z;
	blocks = data;
	if (!createMesh())
		return false;
	return true;
}

bool
vox::Chunk::initGL(void)
{
	glGenBuffers(1, &vboIdx);
	glGenBuffers(1, &vboUV);
	return true;
}

bool
vox::Chunk::__destroy(void)
{
	if (_indicesArray != nullptr)
		delete _indicesArray;
	if (_uvsArray != nullptr)
		delete _uvsArray;
	if (blocks != nullptr)
		delete[] blocks;
	if (vboIdx != -1)
		glDeleteBuffers(1, &vboIdx);
	if (vboUV != -1)
		glDeleteBuffers(1, &vboUV);
	return true;
}

vox::Chunk&
vox::Chunk::operator=(const vox::Chunk& right)
{
	return *this;
	(void)right;
}

bool
vox::Chunk::update(float dt)
{
	(void)dt;
	return true;
}

bool
vox::Chunk::Render(GLint attrCoord3D, GLint attrVertexUV, GLint attrVertexNormal,
					   GLint attrUniformModel, Frustum& frustum, GLint attrTexture)
{
	
	if (_nbTriangle <= 0)
		return true;
	if (!frustum.BoxInFrustum(_x* Chunk::CHUNK_SIZE * 2, _y* Chunk::CHUNK_SIZE * 2, _z* Chunk::CHUNK_SIZE * 2, _x* Chunk::CHUNK_SIZE * 2 + Chunk::CHUNK_SIZE * 2,
							  _y * Chunk::CHUNK_SIZE * 2+ Chunk::CHUNK_SIZE * 2, _z* Chunk::CHUNK_SIZE * 2 + Chunk::CHUNK_SIZE * 2) != Frustum::Outside)
		return true;
	glm::mat4	Model;
	Model = glm::translate(glm::mat4(), glm::vec3(_x * Chunk::CHUNK_SIZE * 2, _y * Chunk::CHUNK_SIZE * 2, _z * Chunk::CHUNK_SIZE * 2)) *
			glm::scale((float)Chunk::CHUNK_SCALE, (float)Chunk::CHUNK_SCALE, (float)Chunk::CHUNK_SCALE);

	glUniformMatrix4fv(attrUniformModel, 1, GL_FALSE, &Model[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures);
	glUniform1i(attrTexture, GL_TEXTURE0);

	glEnableVertexAttribArray(attrCoord3D);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
	glVertexAttribPointer(attrCoord3D, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	glEnableVertexAttribArray(attrVertexNormal);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
	glVertexAttribPointer(attrVertexNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(attrVertexUV);
	glBindBuffer(GL_ARRAY_BUFFER, vboUV);
	glVertexAttribPointer(attrVertexUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx);
	glDrawElements(GL_TRIANGLES, _nbTriangle * 6, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(attrVertexUV);
	glDisableVertexAttribArray(attrVertexNormal);
	glDisableVertexAttribArray(attrCoord3D);
	return true;
}

bool
vox::Chunk::createMesh(void)
{
	GLuint IArray[36] = {
		0,1,2,2,1,3,
		4,5,6,6,5,7,

		11,9,13,13,9,15,
		8,10,14,14,10,12,

		22,23,16,16,23,17,
		18,19,20,20,19,21
	};

	if (_indicesArray != nullptr || _uvsArray != nullptr)
		return false;
	_indicesArray = new (std::nothrow) GLuint[36 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
	if (_indicesArray == nullptr)
		return false;
	int	i = 0;

	_nbTriangle = 0;
	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (int y = 0; y < CHUNK_SIZE; ++y)
		{
			for (int z = 0; z < CHUNK_SIZE; ++z)
			{
				if (blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE] != 0)
					createCube(_indicesArray, IArray, i, x, y, z);
				++i;
			}
		}
	}

	GLfloat	coArrayUV[48] =
	{
		0.0f, 0.0f,			//0
		0.0f, 0.0625f,		//1
		0.0625f, 0.0f,		//2
		0.0625f, 0.0625f,	//3

		0.0f, 0.0f,			//4
		0.0f, 0.0625f,		//5
		0.0625f, 0.0f,		//6
		0.0625f, 0.0625f,	//7
		
		0.0f, 0.0f,			//8
		0.0f, 0.0625f,		//9
		0.0625f, 0.0f,		//10
		0.0625f, 0.0625f,	//11
		
		0.0625f, 0.0625f,	//12
		0.0625f, 0.0f,		//13
		0.0f, 0.0625f,		//14
		0.0f, 0.0f,			//15
		
		0.0f, 0.0f,			//16
		0.0f, 0.0625f,		//17
		0.0625f, 0.0f,		//18
		0.0625f, 0.0625f,	//19
		
		0.0f, 0.0f,			//20
		0.0f, 0.0625f,		//21
		0.0625f, 0.0f,		//22
		0.0625f, 0.0625f,	//23
	};
	_uvsArray = new (std::nothrow) GLfloat[48 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
	if (_uvsArray == nullptr)
		return false;
	glm::vec2 typeOffset[255];
	for(int i = 0; i < 255; ++i)
	{
		typeOffset[i].x = 4 * 0.0625f;
		typeOffset[i].y = 12 * 0.0625f;
	}
	typeOffset[vox::Cube::STONE].x = 0.0f * 0.0625f; typeOffset[vox::Cube::STONE].y = 0.0f * 0.0625f;
	typeOffset[vox::Cube::DIRT].x =  2.0f * 0.0625f; typeOffset[vox::Cube::DIRT].y = 0.0f * 0.0625f;
	typeOffset[vox::Cube::GRASS].x =  2.0f * 0.0625f; typeOffset[vox::Cube::GRASS].y = 0.0f * 0.0625f;
	typeOffset[vox::Cube::SAND].x =  0.0f * 0.0625f; typeOffset[vox::Cube::SAND].y = 11.0f * 0.0625f;
	typeOffset[vox::Cube::WATER_SOURCE].x =  14.0f * 0.0625f; typeOffset[vox::Cube::WATER_SOURCE].y = 12.0f * 0.0625f;
	typeOffset[vox::Cube::WATER_FLOWING].x =  14.0f * 0.0625f; typeOffset[vox::Cube::WATER_FLOWING].y = 12.0f * 0.0625f;
	i = 0;
	for (int x = 0 ; x < CHUNK_SIZE ; ++x)
	{
		for (int y = 0 ; y < CHUNK_SIZE ; ++y)
		{
			for (int z = 0 ; z < CHUNK_SIZE ; ++z)
			{
				unsigned int id = blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE];
				for (int j = 0; j < 48; ++j)
				{
					if (id == vox::Cube::GRASS && (j == 16 || j == 17 || j == 20 || j == 21 || j == 24 || j == 25 || j == 28 || j == 29 ))
					{//16 17 20 21
						if (j % 2 == 0)
							_uvsArray[i * 48 + j] = coArrayUV[j] + 3.0f * 0.0625f;
						else
							_uvsArray[i * 48 + j] = coArrayUV[j] / 2.0f;
					}
					else
					{
						if (j % 2 == 0)
							_uvsArray[i * 48 + j] = coArrayUV[j] + typeOffset[id].x;
						else
							_uvsArray[i * 48 + j] = coArrayUV[j] + typeOffset[id].y;
					}
				}
				++i;
			}
		}
	}
	return true;
}

void
vox::Chunk::bindMesh(void)
{
	if (_nbTriangle > 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIdx);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _nbTriangle * 6, _indicesArray, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, vboUV);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 48 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, _uvsArray, GL_DYNAMIC_DRAW);
	}
	delete[] _indicesArray;
	_indicesArray = nullptr;
	delete[] _uvsArray;
	_uvsArray = nullptr;
}

bool
vox::Chunk::createCube(GLuint* indice, GLuint IArray[], int i, int x, int y, int z)
{
	if (x - 1 < 0 || !blocks[x - 1 + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE])
		copyIndice(indice, IArray, i, 0);
	if (x + 1 >= CHUNK_SIZE || !blocks[x + 1 + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE])
		copyIndice(indice, IArray, i, 6);
	if (y - 1 < 0 || !blocks[x + z * CHUNK_SIZE + (y - 1) * CHUNK_SIZE * CHUNK_SIZE])
		copyIndice(indice, IArray, i, 12);
	if (y + 1 >= CHUNK_SIZE || !blocks[x + z * CHUNK_SIZE + (y + 1) * CHUNK_SIZE * CHUNK_SIZE])
		copyIndice(indice, IArray, i, 18);
	if (z - 1 < 0 || !blocks[x + (z - 1) * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE])
		copyIndice(indice, IArray, i, 24);
	if (z + 1 >= CHUNK_SIZE || !blocks[x + (z + 1) * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE])
		copyIndice(indice, IArray, i, 30);
	return true;
}

void
vox::Chunk::copyIndice(GLuint* indice, GLuint IArray[], int i, int offset)
{
	for (int j = 0 ; j < 6 ; ++j)
	{
		indice[_nbTriangle * 6 + j] = IArray[offset + j] + i * 24;
	}
	++_nbTriangle;
}

bool
vox::Chunk::blockIsActive(int x, int y, int z)
{
	if (x >= 0 && x < CHUNK_SIZE && y >= 0 &&
	    y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE &&
	    blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE])
		return true;
	return false;
}

int
vox::Chunk::getPosX(void)
{
	return _x;
}

int
vox::Chunk::getPosY(void)
{
	return _y;
}

int
vox::Chunk::getPosZ(void)
{
	return _z;
}

void
vox::Chunk::getPosition(glm::vec3& res)
{
	res = glm::vec3(_x, _y, _z);
}

void
vox::Chunk::getPosition(Vector3Int& res)
{
	res = Vector3Int(_x, _y, _z);
}

void
vox::Chunk::setBlockActive(int x, int y, int z, unsigned int id)
{
	blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE] = id;
}

int
vox::Chunk::GetBlockID(int x, int y, int z)
{
	return blocks[x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE];
}

bool
vox::Chunk::UpdateBlockAt(int x, int y, int z, int id)
{
	if (x >= 0 && y >= 0 && z >= 0)
	{
		if (x >= 16)
			x = 15;
		if (z >= 16)
			z = 15;
		setBlockActive(x, y, z, id);
		createMesh();
		bindMesh();
	}
	return true;		
}