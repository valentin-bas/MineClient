#pragma once

#include "AbstractRenderer.h"
#include <OVR.h>

namespace vox
{
	class Frustum;
	class OculusRenderer : public AbstractRenderer
	{
	private:
		bool			__Render(Frustum& frustum, bool side, std::map<Vector3Int, Chunk*>& chunckMap, std::map<int, Entity*>& entities,
							     ParticleWorld& world, glm::mat4& view, glm::mat4& proj);
		
		bool			__SetOculusUniform(const OVR::Util::Render::StereoEyeParams& eye);
	protected:
		GLuint		OculusProgram;
		GLint		attrOculusLensCenter;
		GLint		attrOculusScreenCenter;
		GLint		attrOculusScale;
		GLint		attrOculusScaleIn;
		GLint		attrOculusHmdWarpParam;
		GLint		attrOculusVertexUV;
		GLint		attrOculusSide;

		OVR::Util::Render::StereoConfig*	SConfig;

	public:
						OculusRenderer(void);
		virtual			~OculusRenderer(void);
		virtual bool	Init(int width, int height);
		virtual bool	InitShader(void);
		virtual bool	Render(std::map<Vector3Int, Chunk*>& chunckMap, std::map<int, Entity*>& entities,
							   ParticleWorld& particles, glm::mat4& view, glm::mat4& proj) override;

		bool			SetOculusConfig(OVR::HMDInfo& info);

		// Inline
		OVR::Util::Render::StereoConfig*	GetStereoConfig() { return SConfig; }
	};
};