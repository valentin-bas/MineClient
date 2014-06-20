#ifndef __ID_OBJ_H__
#define __ID_OBJ_H__

#pragma once
#ifdef MINEENGINE_EXPORTS
#define MINEENGINE_API __declspec(dllexport)
#else
#define MINEENGINE_API __declspec(dllimport)
#endif


#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>

namespace id
{
	class MINEENGINE_API Obj
	{
		typedef float GLfloat;
		typedef unsigned int GLuint;
	public:
		Obj(void);
		virtual ~Obj(void);

		bool	loadObj(const char* path);
		void	GetRenderInfo(GLuint& texture, GLuint& vertices,
							  GLuint& uvs, GLuint& normals, unsigned int& triangleCount);
	private:
		GLuint	_textureId, _vboVertex, _vboUV, _vboNormal;
		unsigned int _triangleCount;
	};
}
#endif
