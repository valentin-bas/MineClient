//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "sparticules_updater_life.h"

//--------------------------------------------------------------------------------------

void	ParticleUpdaterLife::Update(ParticleStream &stream, float dt) const
{
	for (u32 i = 0; i < stream.m_Count; i++)
	{
		float	life = stream.m_Lives[i];
		float	invLife = 1.0f / life;	// hint: can store invLife instead of life, and only do the rcp() when initializing the particle.

		stream.m_LifeRatios[i] += dt * invLife;
		if (stream.m_LifeRatios[i] >= 1.0f)	// particle is dead
		{
			stream.Remove(i);
			--i;
		}
	}
}

//--------------------------------------------------------------------------------------

