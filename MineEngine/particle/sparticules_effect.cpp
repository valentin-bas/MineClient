//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "sparticules_interface.h"

//--------------------------------------------------------------------------------------
//
//	particle effect implementation
//
//--------------------------------------------------------------------------------------

ParticleEffect::~ParticleEffect()
{
	DeleteAllEmitters();
}

//--------------------------------------------------------------------------------------

void	ParticleEffect::DeleteAllEmitters()
{
	for (size_t i = 0, stop = m_Emitters.size(); i < stop; i++)
	{
		delete m_Emitters[i];
	}
	m_Emitters.clear();
}

//--------------------------------------------------------------------------------------

bool	ParticleEffect::AddEmitter(ParticleEmitter *emitter)
{
	if (emitter == nullptr)
		return false;

	size_t	prevCount = m_Emitters.size();
	m_Emitters.push_back(emitter);
	if (m_Emitters.size() == prevCount + 1)
		return true;

	delete emitter;	// the caller has given us ownership
	return false;
}

//--------------------------------------------------------------------------------------

bool	ParticleEffect::Update(float dt)
{
	for (u32 i = 0; i < m_Emitters.size(); i++)
	{
		if (!m_Emitters[i]->Update(dt))
		{
			delete m_Emitters[i];
			m_Emitters[i] = m_Emitters[m_Emitters.size() - 1];
			m_Emitters.pop_back();
			--i;
		}
	}
	return !m_Emitters.empty();
}

//--------------------------------------------------------------------------------------

