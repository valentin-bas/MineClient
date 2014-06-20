#pragma once

#include "AbstractRenderer.h"

namespace vox
{
	class Frustum;
	class BasicRenderer : public AbstractRenderer
	{
	private:
		bool			__Render(Frustum& frustum, std::map<Vector3Int, Chunk*>& chunckMap,
								 std::map<int, Entity*>& entities, ParticleWorld& particles,
								 glm::mat4& view, glm::mat4& proj,
								 GLint attrCoord, GLint attrVP, GLint attrModel);
	protected:
		GLuint		OculusProgram;
		GLint		attrOculusVertexUV;

	public:
						BasicRenderer(void);
		virtual			~BasicRenderer(void);
		virtual bool	Init(int width, int height);
		virtual bool	InitShader(void);
		virtual bool	Render(std::map<Vector3Int, Chunk*>& chunckMap, std::map<int, Entity*>& entities,
							   ParticleWorld& particles, glm::mat4& view, glm::mat4& proj);
	};
};