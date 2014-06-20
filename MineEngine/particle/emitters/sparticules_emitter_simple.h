#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_EMITTER_SIMPLE_H__
#define	__SPARTICULES_EMITTER_SIMPLE_H__

#include "../sparticules_interface.h"

//--------------------------------------------------------------------------------------
//
//	simple 'stream' emitter. emits a given rate of particles for a certain amount of time.
//
//--------------------------------------------------------------------------------------

class	SimpleParticleEmitter : public ParticleEmitter
{
protected:
	ParticleLayer		*m_TargetLayer;
	float				m_RatePerSecond;
	float				m_Duration;
	float3				m_Location;
	float				m_EmitTimeLeft;
	float				m_EmitCountAccum;

public:
	SimpleParticleEmitter(ParticleLayer *targetLayer, float ratePerSecond, float duration, const float3 &location);

	virtual bool		Update(float dt) override;
};

//--------------------------------------------------------------------------------------

#endif	// __SPARTICULES_EMITTER_SIMPLE_H__

