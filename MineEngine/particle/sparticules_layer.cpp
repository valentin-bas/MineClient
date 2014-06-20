//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "sparticules_layer.h"
#include "common\sparticules_simple_vectormath.h"

//--------------------------------------------------------------------------------------
//
//	Particle layer implementation
//
//--------------------------------------------------------------------------------------

SimpleParticleLayer::SimpleParticleLayer(CbSetupParticle setupParticle)
:	m_Particles(this)
,	m_SetupParticle(setupParticle)
{
	assert(m_SetupParticle != nullptr);
}

SimpleParticleLayer::~SimpleParticleLayer()
{
	DeleteAllUpdaters();
	DeleteAllRenderers();
}

//--------------------------------------------------------------------------------------

void	SimpleParticleLayer::DeleteAllUpdaters()
{
	for (size_t i = 0, stop = m_Updaters.size(); i < stop; i++)
	{
		delete m_Updaters[i];
	}
	m_Updaters.clear();
}

//--------------------------------------------------------------------------------------

bool	SimpleParticleLayer::AddUpdater(ParticleUpdater *updater)
{
	if (updater == nullptr)
		return false;

	size_t	prevCount = m_Updaters.size();
	m_Updaters.push_back(updater);
	if (m_Updaters.size() == prevCount + 1)
		return true;

	delete updater;	// the caller has given us ownership
	return false;
}

//--------------------------------------------------------------------------------------

void	SimpleParticleLayer::DeleteAllRenderers()
{
	for (size_t i = 0, stop = m_Renderers.size(); i < stop; i++)
	{
		delete m_Renderers[i];
	}
	m_Renderers.clear();
}

//--------------------------------------------------------------------------------------

bool	SimpleParticleLayer::AddRenderer(ParticleRenderer *renderer)
{
	if (renderer == nullptr)
		return false;

	size_t	prevCount = m_Renderers.size();
	m_Renderers.push_back(renderer);
	if (m_Renderers.size() == prevCount + 1)
		return true;

	delete renderer;	// the caller has given us ownership
	return false;
}

//--------------------------------------------------------------------------------------

u32		SimpleParticleLayer::Count() const
{
	return m_Particles.m_Count;
}

//--------------------------------------------------------------------------------------

void	SimpleParticleLayer::Clear()
{
	m_Particles.Clear();
}

//--------------------------------------------------------------------------------------

void	SimpleParticleLayer::Update(float dt)
{
	// pre-update step: build 'PrevPositions'
	static_assert(sizeof(*m_Particles.m_PrevPositions) == sizeof(*m_Particles.m_Positions), "mismatching stream types: cannot copy 'Positions' into 'PrevPositions'");
	memcpy(m_Particles.m_PrevPositions, m_Particles.m_Positions, m_Particles.m_Count * sizeof(*m_Particles.m_PrevPositions));

	for (size_t i = 0, stop = m_Updaters.size(); i < stop; i++)
	{
		m_Updaters[i]->Update(m_Particles, dt);
	}
}

//--------------------------------------------------------------------------------------

void	SimpleParticleLayer::Render(const glm::mat4& matWV, const glm::mat4& matWVP, GLint attrCoord3D,
								    GLint attrVertexUV, GLint attrVertexNormal, GLint attrUniformModel, GLint attrTexture) const
{
	for (size_t i = 0, stop = m_Renderers.size(); i < stop; i++)
	{
		m_Renderers[i]->Render(m_Particles, matWV, matWVP, attrCoord3D, attrVertexUV,
							   attrVertexNormal, attrUniformModel, attrTexture);
	}
}

//--------------------------------------------------------------------------------------

HRESULT	SimpleParticleLayer::CreateComponents(void)
{
	HRESULT	hr = S_OK;
	for (size_t i = 0, stop = m_Renderers.size(); i < stop; i++)
	{
		V_RETURN(m_Renderers[i]->CreateComponents());
	}
	return hr;
}

//--------------------------------------------------------------------------------------

void	SimpleParticleLayer::DestroyComponents()
{
	for (size_t i = 0, stop = m_Renderers.size(); i < stop; i++)
	{
		m_Renderers[i]->DestroyComponents();
	}
}

//--------------------------------------------------------------------------------------

void	SimpleParticleLayer::Emit(u32 count, const float3 &location)
{
	assert(m_SetupParticle != nullptr);

	u32	oldSize = m_Particles.m_Count;
	u32	newSize = oldSize + count;
	if (!m_Particles.Resize(newSize))	// failed resizing
		return;

	m_SetupParticle(m_Particles, location, oldSize, newSize);
}

//--------------------------------------------------------------------------------------

