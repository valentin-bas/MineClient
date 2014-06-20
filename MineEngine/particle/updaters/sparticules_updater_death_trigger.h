#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_UPDATER_DEATH_TRIGGER_H__
#define	__SPARTICULES_UPDATER_DEATH_TRIGGER_H__

#include "../sparticules_layer.h"

//--------------------------------------------------------------------------------------

class	ParticleUpdaterDeathTrigger : public ParticleUpdater
{
public:
	typedef void	(*CbOnTrigger)(ParticleWorld *world, void *triggerData, const float3 &location);
protected:
	CbOnTrigger		m_Trigger;
	void			*m_TriggerData;
public:
	ParticleUpdaterDeathTrigger(CbOnTrigger onTrigger, void *triggerData);

	virtual void	Update(ParticleStream &stream, float dt) const override;
};

//--------------------------------------------------------------------------------------

#endif	// __SPARTICULES_UPDATER_DEATH_TRIGGER_H__

