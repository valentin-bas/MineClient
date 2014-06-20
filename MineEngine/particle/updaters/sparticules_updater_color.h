#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_UPDATER_COLOR_H__
#define	__SPARTICULES_UPDATER_COLOR_H__

#include "../sparticules_layer.h"

//--------------------------------------------------------------------------------------

class	ParticleUpdaterColor : public ParticleUpdater
{
protected:
	s16			m_ColorStart[4];
	s16			m_ColorDelta[4];

public:
	ParticleUpdaterColor(u32 colorStart, u32 colorEnd);

	virtual void	Update(ParticleStream &stream, float dt) const override;
};

//--------------------------------------------------------------------------------------

#endif	// __SPARTICULES_UPDATER_COLOR_H__

