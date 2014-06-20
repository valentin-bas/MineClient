#include "stdafx.h"
#include "OculusRenderer.h"
#include "Frustum.h"
#include "Chunk.h"
#include "Model3.h"
#include "Vector3Int.h"

vox::OculusRenderer::OculusRenderer(void)
{
}


vox::OculusRenderer::~OculusRenderer(void)
{
	delete SConfig;
}

bool
vox::OculusRenderer::Init(int width, int height)
{
	if (AbstractRenderer::Init(width, height) == false)
		return false;
	if (InitShader() == false)
		return false;
	SConfig = new (std::nothrow) OVR::Util::Render::StereoConfig();
	if (SConfig == nullptr)
		return false;
	return true;
}

bool
vox::OculusRenderer::InitShader(void)
{
	if (CreateProgram("cubeOVR.v.glsl", "cubeOVR.f.glsl", &OculusProgram) == false)
		return false;

	attrOculusVertexUV = glGetAttribLocation(OculusProgram, "coord3d");
	attrOculusLensCenter = glGetUniformLocation(OculusProgram, "occulusBuffer.LensCenter");
	attrOculusScreenCenter = glGetUniformLocation(OculusProgram, "occulusBuffer.ScreenCenter");
	attrOculusScale = glGetUniformLocation(OculusProgram, "occulusBuffer.Scale");
	attrOculusScaleIn = glGetUniformLocation(OculusProgram, "occulusBuffer.ScaleIn");
	attrOculusHmdWarpParam = glGetUniformLocation(OculusProgram, "occulusBuffer.HmdWarpParam");
	attrOculusSide = glGetUniformLocation(OculusProgram, "side");
	
	if (attrOculusSide == -1 ||
		attrOculusVertexUV == -1 || attrOculusHmdWarpParam == -1 || attrOculusScaleIn == -1 ||
		attrOculusScale == -1 || attrOculusScreenCenter == -1 || attrOculusLensCenter == -1)
		return false;
	return true;
}

bool
vox::OculusRenderer::Render(std::map<Vector3Int, Chunk*>& chunckMap, std::map<int, Entity*>& entities,
							ParticleWorld& particles, glm::mat4& view, glm::mat4& proj)
{
	Frustum	frustrum;

	frustrum.ExtractPlanes(proj * view);

	//First pass
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	glUniform1f(attrTime, GetTickCount() / 20.0f);
	
	glViewport(0, 0, _width / 2, _height);
	if (__Render(frustrum, true, chunckMap, entities, particles, view, proj) == false)
		return false;
	glViewport(_width / 2, 0, _width / 2, _height);
	if (__Render(frustrum, false, chunckMap, entities, particles, view, proj) == false)
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
	glBindSampler(renderTextureOdd, sampler);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderTextureOdd);
	__SetOculusUniform(SConfig->GetEyeRenderParams(OVR::Util::Render::StereoEye_Left));
	glViewport(0, 0, _width / 2, _height);
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

	glViewport(_width / 2, 0, _width / 2, _height);
	__SetOculusUniform(SConfig->GetEyeRenderParams(OVR::Util::Render::StereoEye_Right));
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
vox::OculusRenderer::SetOculusConfig(OVR::HMDInfo& info)
{
	SConfig->SetFullViewport(OVR::Util::Render::Viewport(0, 0, info.HResolution, info.VResolution));
	SConfig->SetStereoMode(OVR::Util::Render::Stereo_LeftRight_Multipass);
	SConfig->SetHMDInfo(info);
	SConfig->SetDistortionFitPointVP(-1.0f, 0.0f);
	return true;
}

bool
vox::OculusRenderer::__Render(Frustum& frustum, bool side, std::map<Vector3Int, Chunk*>& chunckMap, std::map<int, Entity*>& entities,
							  ParticleWorld& world, glm::mat4& view, glm::mat4& proj)
{
	glm::mat4	baseProj;
	glm::mat4	diffView;
	float halfIPD = SConfig->GetHMDInfo().InterpupillaryDistance * 0.5f;
	float viewCenter = SConfig->GetHMDInfo().HScreenSize * 0.25f;
	float eyeProjectionShift = viewCenter - SConfig->GetHMDInfo().LensSeparationDistance*0.5f;
	float projectionCenterOffset = 4.0f * eyeProjectionShift / SConfig->GetHMDInfo().HScreenSize;
	glm::vec3 camRight = glm::vec3(1.0f, 0.0f, 0.0f);//view[0][0], view[1][0], view[2][0]);
	if (side)
	{
		baseProj = glm::translate(camRight * projectionCenterOffset) * proj;
		diffView = glm::translate(camRight * halfIPD) * view;
	}		
	else	
	{			
		baseProj = glm::translate(camRight * -projectionCenterOffset) * proj;
		diffView = glm::translate(camRight * -halfIPD) * view;
	}
	glm::mat4	VP = baseProj * diffView;
	glUniformMatrix4fv(attrUniformViewProj, 1, GL_FALSE, &VP[0][0]);
	for (std::map<Vector3Int, Chunk*>::iterator it = chunckMap.begin(); it != chunckMap.end(); ++it)
		if (!it->second->Render(attrCoord3D, attrVertexUV, attrVertexNormal, attrUniformModel, frustum, attrTexture))
			return false;
	for (std::map<int, Entity*>::iterator it = entities.begin(); it != entities.end(); ++it)
		if (!it->second->Render(attrCoord3D, attrVertexUV, attrVertexNormal, attrUniformModel, attrTexture))
			return false;
	return true;
}

bool
vox::OculusRenderer::__SetOculusUniform(const OVR::Util::Render::StereoEyeParams& eye)
{
	const OVR::Util::Render::Viewport*			VP;
	const OVR::Util::Render::DistortionConfig*	distortion;
	const OVR::HMDInfo&							info = SConfig->GetHMDInfo();
	VP = &eye.VP;
	distortion = eye.pDistortion;
	
	float w = float(VP->w) / float(info.HResolution),
          h = float(VP->h) / float(info.VResolution),
          x = float(VP->x) / float(info.HResolution),
          y = float(VP->y) / float(info.VResolution);
	float as = float(VP->w) / float(VP->h);
	float scaleFactor = 1.0f / distortion->Scale;
	float centerOffset = distortion->XCenterOffset;
	if (eye.Eye == OVR::Util::Render::StereoEye_Left)
		glUniform1i(attrOculusSide, 0);
	else
	{
		glUniform1i(attrOculusSide, 1);
		centerOffset = -centerOffset;
	}
	glUniform2f(attrOculusLensCenter, x + (w + centerOffset * 0.5f) * 0.5f, y + h * 0.5f);
	glUniform2f(attrOculusScreenCenter, x + w * 0.5f, y + h * 0.5f);
	glUniform2f(attrOculusScale, (w / 2) * scaleFactor, (h / 2) * scaleFactor * as);
	glUniform2f(attrOculusScaleIn, (2 / w), (2 / h) / as);
	glUniform4f(attrOculusHmdWarpParam, distortion->K[0], distortion->K[1], distortion->K[2], distortion->K[3]);
	return true;
}