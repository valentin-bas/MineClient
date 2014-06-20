#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_UPDATER_COLLISION_H__
#define	__SPARTICULES_UPDATER_COLLISION_H__

#include "../sparticules_layer.h"

//--------------------------------------------------------------------------------------

class	ParticleUpdaterCollision : public ParticleUpdater
{
protected:
	float4			m_PlaneEquation;
	float			m_Restitution;

public:
	ParticleUpdaterCollision(const float3 &planeNormal, float planeDistanceToOriginAlongNormal, float restitution);

	virtual void	Update(ParticleStream &stream, float dt) const override;
};

//--------------------------------------------------------------------------------------

#endif	// __SPARTICULES_UPDATER_COLLISION_H__

