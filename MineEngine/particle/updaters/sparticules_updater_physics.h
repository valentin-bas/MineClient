#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_UPDATER_PHYSICS_H__
#define	__SPARTICULES_UPDATER_PHYSICS_H__

#include "../sparticules_layer.h"

//--------------------------------------------------------------------------------------

class	ParticleUpdaterPhysics : public ParticleUpdater
{
protected:
	float3			m_Gravity;

public:
	ParticleUpdaterPhysics(const float3 &gravity);

	virtual void	Update(ParticleStream &stream, float dt) const override;
};

//--------------------------------------------------------------------------------------

#endif	// __SPARTICULES_UPDATER_PHYSICS_H__

