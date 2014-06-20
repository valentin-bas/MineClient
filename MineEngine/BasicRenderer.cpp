#include "stdafx.h"
#include "BasicRenderer.h"
#include "Frustum.h"
#include "Chunk.h"
#include "Model3.h"
#include "Vector3Int.h"

vox::BasicRenderer::BasicRenderer(void)
{
}


vox::BasicRenderer::~BasicRenderer(void)
{
}

bool
vox::BasicRenderer::Init(int width, int height)
{
	if (AbstractRenderer::Init(width, height) == false)
		return false;
	if (InitShader() == false)
		return false;
	return true;
}

bool
vox::BasicRenderer::InitShader(void)
{
	if (CreateProgram("cubePP.v.glsl", "cubePP.f.glsl", &OculusProgram) == false)
		return false;

	attrOculusVertexUV = glGetAttribLocation(OculusProgram, "coord3d");	
	if (attrOculusVertexUV == -1)
		return false;
	return true;
}

bool
vox::BasicRenderer::Render(std::map<Vector3Int, Chunk*>& chunckMap, std::map<int, Entity*>& entities,
						   ParticleWorld& particles, glm::mat4& view, glm::mat4& proj)
{
	Frustum	frustrum;

	frustrum.ExtractPlanes(proj * view);
	//First pass
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	//glUniform1f(attrTime, GetTickCount() / 20.0f);
	glUniform1f(attrTime, 6000);
	glViewport(0, 0, _width, _height);
	if (__Render(frustrum, chunckMap, entities, particles, view, proj, attrCoord3D, attrUniformViewProj, attrUniformModel) == false)
		return false;
	glBindSampler(0, 0);
	// Second pass
	glDepthMask(GL_FALSE);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextureOdd, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(AAProgram);
	glBindSampler(renderTexture, sampler);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glViewport(0, 0, _width, _height);
	glEnableVertexAttribArray(attrOculusVertexUV);
	glBindBuffer(GL_ARRAY_BUFFER, PostProcessVertexBuffer);
	glVertexAttribPointer(
		attrOculusVertexUV,// attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(attrOculusVertexUV);
	glBindSampler(0, 0);
	glDepthMask(GL_TRUE);
	//Post Process
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(OculusProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindSampler(renderTextureOdd, sampler);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderTextureOdd);
	glViewport(0, 0, _width, _height);
	glEnableVertexAttribArray(attrOculusVertexUV);
	glBindBuffer(GL_ARRAY_BUFFER, PostProcessVertexBuffer);
	glVertexAttribPointer(
		attrOculusVertexUV,// attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(attrOculusVertexUV);
	return true;
}

bool
vox::BasicRenderer::__Render(Frustum& frustum, std::map<Vector3Int, Chunk*>& chunckMap,
							std::map<int, Entity*>& entities, ParticleWorld& particles,
							glm::mat4& view, glm::mat4& proj,
							GLint attrCoord, GLint attrVP, GLint attrModel)
{
	glm::mat4	VP = proj * view;
	glUniformMatrix4fv(attrVP, 1, GL_FALSE, &VP[0][0]);
	for (std::map<int, Entity*>::iterator it = entities.begin(); it != entities.end(); ++it)
		if (!it->second->Render(attrCoord, attrVertexUV, attrVertexNormal, attrModel, attrTexture))
			return false;
	for (std::map<Vector3Int, Chunk*>::iterator it = chunckMap.begin(); it != chunckMap.end(); ++it)
		if (!it->second->Render(attrCoord, attrVertexUV, attrVertexNormal, attrModel, frustum, attrTexture))
			return false;
	particles.Render(view, proj, attrCoord, attrVertexUV, attrVertexNormal, attrUniformModel, attrTexture);
	return true;
}