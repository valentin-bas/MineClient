#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_STREAM_H__
#define	__SPARTICULES_STREAM_H__

#include "sparticules_interface.h"

//--------------------------------------------------------------------------------------

class	ParticleStream
{
public:
	ParticleStream(ParticleLayer *owner);
	~ParticleStream();

	ParticleLayer		*m_Owner;

	float				*m_Lives;
	float				*m_LifeRatios;
	float				*m_Sizes;
	float3				*m_PrevPositions;
	float3				*m_Positions;
	float3				*m_Velocities;
	u32					*m_Colors;

	u32					m_Count;

	void				Clear();
	bool				Resize(u32 newCount);
	void				Remove(u32 index);
};

//--------------------------------------------------------------------------------------

#endif	// __SPARTICULES_STREAM_H__

