//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "common\sparticules_simple_vectormath.h"
#include "sparticules_stream.h"

//--------------------------------------------------------------------------------------
//
//	Particle stream implementation
//
//--------------------------------------------------------------------------------------

ParticleStream::ParticleStream(ParticleLayer *owner)
:	m_Owner(owner)
,	m_Lives(nullptr)
,	m_LifeRatios(nullptr)
,	m_Sizes(nullptr)
,	m_Positions(nullptr)
,	m_PrevPositions(nullptr)
,	m_Velocities(nullptr)
,	m_Colors(nullptr)
,	m_Count(0)
{
	assert(owner != nullptr);
}

//--------------------------------------------------------------------------------------

ParticleStream::~ParticleStream()
{
	Clear();
}

//--------------------------------------------------------------------------------------

void	ParticleStream::Clear()
{
	if (m_Lives != nullptr)
		free(m_Lives);
	if (m_LifeRatios != nullptr)
		free(m_LifeRatios);
	if (m_Sizes != nullptr)
		free(m_Sizes);
	if (m_Positions != nullptr)
		free(m_Positions);
	if (m_PrevPositions != nullptr)
		free(m_PrevPositions);
	if (m_Velocities != nullptr)
		free(m_Velocities);
	if (m_Colors != nullptr)
		free(m_Colors);

	m_Lives			= nullptr;
	m_LifeRatios	= nullptr;
	m_Sizes			= nullptr;
	m_Positions		= nullptr;
	m_PrevPositions	= nullptr;
	m_Velocities	= nullptr;
	m_Colors		= nullptr;
	m_Count			= 0;
}

//--------------------------------------------------------------------------------------

bool	ParticleStream::Resize(u32 newCount)
{
	float	*newLiv = static_cast<float*>	(realloc(m_Lives,			newCount * sizeof(*m_Lives)));
	float	*newRat = static_cast<float*>	(realloc(m_LifeRatios,		newCount * sizeof(*m_LifeRatios)));
	float	*newSiz = static_cast<float*>	(realloc(m_Sizes,			newCount * sizeof(*m_Sizes)));
	float3	*newPos = static_cast<float3*>	(realloc(m_Positions,		newCount * sizeof(*m_Positions)));
	float3	*newPPs = static_cast<float3*>	(realloc(m_PrevPositions,	newCount * sizeof(*m_PrevPositions)));
	float3	*newVel = static_cast<float3*>	(realloc(m_Velocities,		newCount * sizeof(*m_Velocities)));
	u32		*newCol = static_cast<u32*>		(realloc(m_Colors,			newCount * sizeof(*m_Colors)));

	if (newLiv != nullptr)
		m_Lives = newLiv;
	if (newRat != nullptr)
		m_LifeRatios = newRat;
	if (newSiz != nullptr)
		m_Sizes = newSiz;
	if (newPos != nullptr)
		m_Positions = newPos;
	if (newPPs != nullptr)
		m_PrevPositions = newPPs;
	if (newVel != nullptr)
		m_Velocities = newVel;
	if (newCol != nullptr)
		m_Colors = newCol;

	if (newLiv == nullptr ||
		newRat == nullptr ||
		newSiz == nullptr ||
		newPos == nullptr ||
		newPPs == nullptr ||
		newVel == nullptr ||
		newCol == nullptr)
	{
		return false;
	}

	m_Count = newCount;
	return true;
}

//--------------------------------------------------------------------------------------

void	ParticleStream::Remove(u32 index)
{
	assert(index < m_Count);

	u32	lastId = --m_Count;

	m_Lives[index]			= m_Lives[lastId];
	m_LifeRatios[index]		= m_LifeRatios[lastId];
	m_Sizes[index]			= m_Sizes[lastId];
	m_Positions[index]		= m_Positions[lastId];
	m_PrevPositions[index]	= m_PrevPositions[lastId];
	m_Velocities[index]		= m_Velocities[lastId];
	m_Colors[index]			= m_Colors[lastId];
}

//--------------------------------------------------------------------------------------

