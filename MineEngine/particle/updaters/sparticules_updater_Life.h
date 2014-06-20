#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_UPDATER_LIFE_H__
#define	__SPARTICULES_UPDATER_LIFE_H__

#include "../sparticules_layer.h"

//--------------------------------------------------------------------------------------

class	ParticleUpdaterLife : public ParticleUpdater
{
public:
	virtual void	Update(ParticleStream &stream, float dt) const override;
};

//--------------------------------------------------------------------------------------

#endif	// __SPARTICULES_UPDATER_LIFE_H__
