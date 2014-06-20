//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "../common/sparticules_simple_vectormath.h"
#include "sparticules_updater_zone.h"

//--------------------------------------------------------------------------------------

ParticleUpdaterZone::ParticleUpdaterZone(const float3 &center, float radius, u32 colorWhenInside, bool reversed)
:	m_Center(center)
,	m_RadiusSquared(radius * radius)
,	m_InsideColor(colorWhenInside)
,	m_Reversed(reversed)
,	m_Kill(false)
{
}

//--------------------------------------------------------------------------------------

ParticleUpdaterZone::ParticleUpdaterZone(const float3 &center, float radius, bool reversed)
:	m_Center(center)
,	m_RadiusSquared(radius * radius)
,	m_InsideColor(0)
,	m_Reversed(reversed)
,	m_Kill(true)
{
}

//--------------------------------------------------------------------------------------

void	ParticleUpdaterZone::Update(ParticleStream &stream, float dt) const
{
	if (m_Kill)
	{
		// when the zone-test is true, kills the particle.
		for (u32 i = 0; i < stream.m_Count; i++)
		{
			float3	particleToCenter = m_Center - stream.m_Positions[i];
			float	dist = dot(particleToCenter, particleToCenter);

			// when reversed is 'true', will go in the 'if' when dist <= radiusSq
			// when reversed is 'false', will go in the 'if' when dist > radiusSq
			if ((dist <= m_RadiusSquared) != m_Reversed)
			{
				stream.Remove(i);
				--i;
			}
		}
	}
	else
	{
		// when the zone test is true, sets the particle color.
		for (u32 i = 0, stop = stream.m_Count; i < stop; i++)
		{
			float3	particleToCenter = m_Center - stream.m_Positions[i];
			float	dist = dot(particleToCenter, particleToCenter);

			// when reversed is 'true', will go in the 'if' when dist <= radiusSq
			// when reversed is 'false', will go in the 'if' when dist > radiusSq
			if ((dist <= m_RadiusSquared) != m_Reversed)
			{
				stream.m_Colors[i] = m_InsideColor;
			}
		}
	}
}

//--------------------------------------------------------------------------------------

