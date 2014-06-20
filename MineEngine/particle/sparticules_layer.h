#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_LAYER_H__
#define	__SPARTICULES_LAYER_H__

#include "sparticules_stream.h"

//--------------------------------------------------------------------------------------

class	ParticleUpdater
{
public:
	virtual ~ParticleUpdater() {}

	virtual void	Update(ParticleStream &stream, float dt) const = 0;	// const -> useful for threading
};

//--------------------------------------------------------------------------------------

class	ParticleRenderer
{
public:
	virtual ~ParticleRenderer() {}

	virtual void	Render(const ParticleStream &stream, const glm::mat4 &matWV, const glm::mat4 &matWVP, GLint attrCoord3D,
						   GLint attrVertexUV, GLint attrVertexNormal, GLint attrUniformModel, GLint attrTexture) = 0;
	virtual HRESULT	CreateComponents(void) = 0;
	virtual void	DestroyComponents() = 0;
};

//--------------------------------------------------------------------------------------

class	SimpleParticleLayer : public ParticleLayer
{
public:
	typedef void					(*CbSetupParticle)(ParticleStream &stream, const float3 &location, u32 indexStart, u32 indexEnd);
protected:
	ParticleStream					m_Particles;
	std::vector<ParticleUpdater*>	m_Updaters;
	std::vector<ParticleRenderer*>	m_Renderers;

	CbSetupParticle					m_SetupParticle;

public:
	SimpleParticleLayer(CbSetupParticle setupParticle);
	~SimpleParticleLayer();

	void							DeleteAllUpdaters();
	bool							AddUpdater(ParticleUpdater *updater);	// transfers ptr ownership to the layer.

	void							DeleteAllRenderers();
	bool							AddRenderer(ParticleRenderer *renderer);	// transfers ptr ownership to the layer.

	virtual u32						Count() const override;
	virtual void					Clear() override;

	virtual void					Emit(u32 count, const float3 &location) override;
	virtual void					Update(float dt) override;

	virtual void					Render(const glm::mat4& matWV, const glm::mat4& matWVP, GLint attrCoord3D,
										   GLint attrVertexUV, GLint attrVertexNormal, GLint attrUniformModel, GLint attrTexture) const override;
	virtual HRESULT					CreateComponents() override;
	virtual void					DestroyComponents() override;
};

//--------------------------------------------------------------------------------------

#endif	__SPARTICULES_LAYER_H__

