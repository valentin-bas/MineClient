//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "sparticules_interface.h"

//--------------------------------------------------------------------------------------
//
//	particle world implementation
//
//--------------------------------------------------------------------------------------

ParticleWorld::ParticleWorld()
{
}

//--------------------------------------------------------------------------------------

ParticleWorld::~ParticleWorld()
{
	DeleteAllEffects();
	DeleteAllLayers();
}

//--------------------------------------------------------------------------------------

void	ParticleWorld::DeleteAllEffects()
{
	for (size_t i = 0, stop = m_Effects.size(); i < stop; i++)
	{
		delete m_Effects[i];
	}
	m_Effects.clear();
}

//--------------------------------------------------------------------------------------

bool	ParticleWorld::AddEffect(ParticleEffect *effect)
{
	if (effect == nullptr)
		return false;

	size_t	prevCount = m_Effects.size();
	m_Effects.push_back(effect);
	if (m_Effects.size() == prevCount + 1)
		return true;

	delete effect;	// the caller has given us ownership
	return false;
}

//--------------------------------------------------------------------------------------

void	ParticleWorld::DeleteAllLayers()
{
	for (size_t i = 0, stop = m_Layers.size(); i < stop; i++)
	{
		delete m_Layers[i];
	}
	m_Layers.clear();
}

//--------------------------------------------------------------------------------------

bool	ParticleWorld::AddLayer(ParticleLayer *layer)
{
	if (layer == nullptr)
		return false;

	size_t	prevCount = m_Layers.size();
	m_Layers.push_back(layer);
	if (m_Layers.size() == prevCount + 1)
	{
		layer->Bind(this);	// bind the layer to us.
		return true;
	}

	delete layer;	// the caller has given us ownership
	return false;
}

//--------------------------------------------------------------------------------------

u32		ParticleWorld::ParticleCount() const
{
	u32	totalCount = 0;
	for (size_t i = 0, stop = m_Layers.size(); i < stop; i++)
	{
		totalCount += m_Layers[i]->Count();
	}
	return totalCount;
}

//--------------------------------------------------------------------------------------

void	ParticleWorld::Clear()
{
	DeleteAllEffects();
	for (size_t i = 0, stop = m_Layers.size(); i < stop; i++)
	{
		m_Layers[i]->Clear();
	}
}

//--------------------------------------------------------------------------------------

void	ParticleWorld::Update(float dt)
{
	// update effects:
	for (u32 i = 0; i < m_Effects.size(); i++)
	{
		if (!m_Effects[i]->Update(dt))	// if the effect has finished, remove it
		{
			delete m_Effects[i];
			m_Effects[i] = m_Effects[m_Effects.size() - 1];
			m_Effects.pop_back();
			--i;
		}
	}

	// then, update all layers:
	for (size_t i = 0, stop = m_Layers.size(); i < stop; i++)
	{
		m_Layers[i]->Update(dt);
	}
}

//--------------------------------------------------------------------------------------

void	ParticleWorld::Render(const glm::mat4& matWV, const glm::mat4& matWVP,
							  GLint attrCoord3D, GLint attrVertexUV, GLint attrVertexNormal,
							  GLint attrUniformModel, GLint attrTexture) const
{
	for (size_t i = 0, stop = m_Layers.size(); i < stop; i++)
	{
		m_Layers[i]->Render(matWV, matWVP, attrCoord3D, attrVertexUV, attrVertexNormal,
							attrUniformModel, attrTexture);
	}
}

//--------------------------------------------------------------------------------------

HRESULT	ParticleWorld::CreateComponents()
{
	HRESULT	hr = S_OK;
	for (size_t i = 0, stop = m_Layers.size(); i < stop; i++)
	{
		V_RETURN(m_Layers[i]->CreateComponents());
	}
	return hr;
}

//--------------------------------------------------------------------------------------

void	ParticleWorld::DestroyComponents()
{
	for (size_t i = 0, stop = m_Layers.size(); i < stop; i++)
	{
		m_Layers[i]->DestroyComponents();
	}
}

//--------------------------------------------------------------------------------------

