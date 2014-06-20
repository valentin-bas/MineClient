#pragma once

#include "Entity.h"

namespace vox
{
	class SkyDome : public Entity
	{
		GLuint	vboVertex;
		GLuint	vboNormal;
		GLuint	vboUV;
		GLuint	vboIdx;

		GLuint	texture;

		int		nbTriangle;

	public:
				SkyDome(void);
				~SkyDome(void);
	
		bool	Init(void);
		bool	InitVertex(void);
		bool	Render(GLint attrCoord3D, GLint attrVertexUV,
					   GLint attrVertexNormal, GLint attrUniformModel, GLint attrTexture);
	};
};