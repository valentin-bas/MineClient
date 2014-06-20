#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_RENDERER_BILLBOARD_H__
#define	__SPARTICULES_RENDERER_BILLBOARD_H__

#include "../sparticules_layer.h"

//--------------------------------------------------------------------------------------

class	ParticleRendererBillboard : public ParticleRenderer
{
protected:
	struct	Vertex
	{
		float3		m_Position;
		u32			m_Color;
		float2		m_UV;
	};

	GLuint	_vbo;
	GLuint	_ibo;
	bool	_buffersCreated;

	u32							m_AvailableVertices;
	std::string					m_TexturePath;

	void						_FillVertexBuffer(const ParticleStream &stream, Vertex *dstBuffer, const glm::mat4 &matWV) const;
	HRESULT						_ResizeBuffers(u32 newParticleCount);
	void						_DestroyComponents();

public:
	ParticleRendererBillboard(const char *texturePath);
	~ParticleRendererBillboard();

	virtual void				Render(const ParticleStream &stream, const glm::mat4 &matWV, const glm::mat4 &matWVP, GLint attrCoord3D,
								    GLint attrVertexUV, GLint attrVertexNormal, GLint attrUniformModel, GLint attrTexture) override;
	virtual HRESULT				CreateComponents() override;
	virtual void				DestroyComponents() override;
};

//--------------------------------------------------------------------------------------

#endif	// __SPARTICULES_RENDERER_BILLBOARD_H__

