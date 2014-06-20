//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "sparticules_updater_physics.h"

//--------------------------------------------------------------------------------------

ParticleUpdaterPhysics::ParticleUpdaterPhysics(const float3 &gravity)
:	m_Gravity(gravity)
{
}

//--------------------------------------------------------------------------------------

void	ParticleUpdaterPhysics::Update(ParticleStream &stream, float dt) const
{
	const float3	acceleration = m_Gravity;
	for (u32 i = 0, stop = stream.m_Count; i < stop; i++)
	{
		stream.m_Velocities[i].x += acceleration.x * dt;
		stream.m_Velocities[i].y += acceleration.y * dt;
		stream.m_Velocities[i].z += acceleration.z * dt;

		stream.m_Positions[i].x += stream.m_Velocities[i].x * dt;
		stream.m_Positions[i].y += stream.m_Velocities[i].y * dt;
		stream.m_Positions[i].z += stream.m_Velocities[i].z * dt;
	}
}

//--------------------------------------------------------------------------------------

