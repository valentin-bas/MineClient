//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "sparticules_emitter_simple.h"

//--------------------------------------------------------------------------------------
//
//	Simple particle emitter implementation
//
//--------------------------------------------------------------------------------------

SimpleParticleEmitter::SimpleParticleEmitter(ParticleLayer *targetLayer, float ratePerSecond, float duration, const float3 &location)
:	m_TargetLayer(targetLayer)
,	m_RatePerSecond(ratePerSecond)
,	m_Duration(duration)
,	m_Location(location)
,	m_EmitTimeLeft(duration)
,	m_EmitCountAccum(0)
{
	assert(m_TargetLayer != nullptr);
}

//--------------------------------------------------------------------------------------

bool	SimpleParticleEmitter::Update(float dt)
{
	assert(m_TargetLayer != nullptr);
	assert(dt >= 0.0f);	// shouldn't be negative !
	assert(m_EmitTimeLeft >= 0.0f);	// return value was not taken into account correctly.

	if (m_Duration == 0.0f)
	{
		u32	emitCount = static_cast<u32>(m_RatePerSecond);
		if (emitCount != 0)
		{
			m_TargetLayer->Emit(emitCount, m_Location);
		}
		m_EmitTimeLeft = -1.0f; // finished
	}
	else
	{
		float	dtLeft = std::min(dt, std::max(m_EmitTimeLeft, 0.0f));
		m_EmitTimeLeft -= dt;

		float	emitCountThisFrame = m_EmitCountAccum + dtLeft * m_RatePerSecond;
		u32		emitCount = static_cast<u32>(emitCountThisFrame);
		m_EmitCountAccum = emitCountThisFrame - static_cast<float>(emitCount);
		if (emitCount != 0)
		{
			m_TargetLayer->Emit(emitCount, m_Location);
		}

		if (m_EmitTimeLeft <= 0.0f)
			m_EmitTimeLeft = -1.0f;	// finished
	}
	return m_EmitTimeLeft >= 0.0f;
}

//--------------------------------------------------------------------------------------

