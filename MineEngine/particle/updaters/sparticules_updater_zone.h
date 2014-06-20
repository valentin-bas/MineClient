#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_UPDATER_ZONE_H__
#define	__SPARTICULES_UPDATER_ZONE_H__

#include "../sparticules_layer.h"

//--------------------------------------------------------------------------------------

class	ParticleUpdaterZone : public ParticleUpdater
{
protected:
	float3			m_Center;
	float			m_RadiusSquared;
	u32				m_InsideColor;
	bool			m_Reversed;
	bool			m_Kill;

public:
	ParticleUpdaterZone(const float3 &center, float radius, u32 colorWhenInside, bool reversed);
	ParticleUpdaterZone(const float3 &center, float radius, bool reversed);

	virtual void	Update(ParticleStream &stream, float dt) const override;
};

//--------------------------------------------------------------------------------------

#endif	// __SPARTICULES_UPDATER_ZONE_H__

