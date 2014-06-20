#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_INTERFACE_H__
#define	__SPARTICULES_INTERFACE_H__

//--------------------------------------------------------------------------------------
// forward-declares

class	ParticleWorld;

//--------------------------------------------------------------------------------------
// abstract emitter

class	ParticleEmitter
{
public:
	virtual			~ParticleEmitter() {}

	virtual bool	Update(float dt) = 0;	// returns true if there are still particles to spawn
};

//--------------------------------------------------------------------------------------
// effect (collection of emitters)

class	ParticleEffect
{
protected:
	std::vector<ParticleEmitter*>	m_Emitters;

public:
	~ParticleEffect();

	void			DeleteAllEmitters();
	bool			AddEmitter(ParticleEmitter *emitter);

	bool			Update(float dt);	// returns true if there are still particles to spawn
};

//--------------------------------------------------------------------------------------
// particle layer, stores one type of particle

class	ParticleLayer
{
protected:
	ParticleWorld			*m_World;

public:
							ParticleLayer() : m_World(nullptr) {}
	virtual					~ParticleLayer() {}

	virtual void			Bind(ParticleWorld *world) { m_World = world; }
	ParticleWorld			*World() const { return m_World; }

	virtual u32				Count() const = 0;
	virtual void			Clear() = 0;
	virtual void			Emit(u32 count, const float3 &location) = 0;
	virtual void			Update(float dt) = 0;

	virtual void			Render(const glm::mat4& matWV, const glm::mat4& matWVP, GLint attrCoord3D,
										   GLint attrVertexUV, GLint attrVertexNormal, GLint attrUniformModel, GLint attrTexture) const = 0;
	virtual HRESULT			CreateComponents(void) = 0;
	virtual void			DestroyComponents() = 0;
};

//--------------------------------------------------------------------------------------
// particle world (collection of layers and effects)

class	ParticleWorld
{
protected:
	std::vector<ParticleLayer*>		m_Layers;
	std::vector<ParticleEffect*>	m_Effects;

public:
	ParticleWorld();
	~ParticleWorld();

	void							DeleteAllEffects();
	bool							AddEffect(ParticleEffect *effect);	// transfers ptr ownership to the particle world.

	void							DeleteAllLayers();
	bool							AddLayer(ParticleLayer *layer);	// transfers ptr ownership to the particle world.

	u32								ParticleCount() const;
	void							Clear();
	void							Update(float dt);

	virtual void					Render(const glm::mat4& matWV, const glm::mat4& matWVP, GLint attrCoord3D, GLint attrVertexUV,
										   GLint attrVertexNormal, GLint attrUniformModel, GLint attrTexture) const;
	virtual HRESULT					CreateComponents(void);
	virtual void					DestroyComponents();
};

//--------------------------------------------------------------------------------------

#endif	__SPARTICULES_INTERFACE_H__

