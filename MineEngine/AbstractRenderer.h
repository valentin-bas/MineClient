#pragma once

#include "particle\sparticules_interface.h"

namespace id
{
	class Obj;
	class Model3;
}

namespace vox
{
	class	Chunk;
	class	ChunkColumn;
	class	Entity;
	class	Vector3Int;
	class AbstractRenderer
	{
	protected:
		//Window Properties
		int			_width;
		int			_height;
		//Post Process
		GLuint		PostProcessVertexBuffer;

		//Shader Stuff
		GLuint		program;
		GLuint		AAProgram;
	
		GLint		attrCoord3D;
		GLint		attrVertexUV;
		GLint		attrVertexNormal;
		GLint		attrUniformCamPos;
		GLint		attrUniformViewProj;
		GLint		attrUniformModel;
		GLint		attrTexture;
		GLuint		sampler;
		GLint		attrTime;
	
		//FirstPass Buffer
		GLuint		frameBuffer;
		GLuint		renderTexture;
		GLuint		renderTextureOdd;
		GLuint		depthBuffer;

		virtual bool	InitShader(void);
	public:
						AbstractRenderer(void);
		virtual			~AbstractRenderer(void);

		virtual bool	Init(int width, int height);
		virtual bool	Render(std::map<Vector3Int, Chunk*>& chunckMap, std::map<int, Entity*>& entities,
							   ParticleWorld& particles, glm::mat4& view, glm::mat4& proj) = 0;


		// Inline
		GLint	GetFrameBuffer() { return frameBuffer; }

	};
};
