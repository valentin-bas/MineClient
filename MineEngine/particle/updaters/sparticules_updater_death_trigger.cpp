//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "sparticules_updater_death_trigger.h"

//--------------------------------------------------------------------------------------

ParticleUpdaterDeathTrigger::ParticleUpdaterDeathTrigger(CbOnTrigger onTrigger, void *triggerData)
:	m_Trigger(onTrigger)
,	m_TriggerData(triggerData)
{
}

//--------------------------------------------------------------------------------------

void	ParticleUpdaterDeathTrigger::Update(ParticleStream &stream, float dt) const
{
	assert(stream.m_Owner != nullptr);
	if (m_Trigger == nullptr)
		return;

	ParticleWorld	*targetWorld = stream.m_Owner->World();
	for (u32 i = 0, stop = stream.m_Count; i < stop; i++)
	{
		float	life = stream.m_Lives[i];
		float	invLife = 1.0f / life;	// hint: can store invLife instead of life, and only do the rcp() when initializing the particle.

		float	lr = stream.m_LifeRatios[i] + dt * invLife;
		if (lr >= 1.0f)	// particle will die this frame.
		{
			m_Trigger(targetWorld, m_TriggerData, stream.m_Positions[i]);	// trigger event
		}
	}
}

//--------------------------------------------------------------------------------------

