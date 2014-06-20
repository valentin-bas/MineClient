//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "../common/sparticules_simple_vectormath.h"
#include "sparticules_renderer_billboard.h"

//--------------------------------------------------------------------------------------

ParticleRendererBillboard::ParticleRendererBillboard(const char *texturePath) :
	m_AvailableVertices(0), m_TexturePath(texturePath), _buffersCreated(false)
{
}

//--------------------------------------------------------------------------------------

ParticleRendererBillboard::~ParticleRendererBillboard()
{
	_DestroyComponents();
}

//--------------------------------------------------------------------------------------

void	ParticleRendererBillboard::_FillVertexBuffer(const ParticleStream &stream, Vertex *dstBuffer, const glm::mat4 &matWV) const
{
	// grab the 'side' and 'up' axes of the view matrix, to perform screen-aligned billboarding:
	const float		*rawWV		= reinterpret_cast<const float*>(&matWV);
	const float3	viewSide	= float3(rawWV[0], rawWV[4], rawWV[8]);
	const float3	viewUp		= float3(rawWV[1], rawWV[5], rawWV[9]);

	for (u32 i = 0, stop = stream.m_Count; i < stop; i++)
	{
		const float3	&pos	= stream.m_Positions[i];
		const u32		color	= stream.m_Colors[i];
		const float		size	= stream.m_Sizes[i];
		const float3	side	= size * viewSide;
		const float3	up		= size * viewUp;

		dstBuffer[0].m_Position	= pos - side + up;
		dstBuffer[1].m_Position	= pos + side + up;
		dstBuffer[2].m_Position	= pos + side - up;
		dstBuffer[3].m_Position	= pos - side - up;

		dstBuffer[0].m_Color	= color;
		dstBuffer[1].m_Color	= color;
		dstBuffer[2].m_Color	= color;
		dstBuffer[3].m_Color	= color;

		dstBuffer[0].m_UV	= float2(0, 0);
		dstBuffer[1].m_UV	= float2(1, 0);
		dstBuffer[2].m_UV	= float2(1, 1);
		dstBuffer[3].m_UV	= float2(0, 1);

		dstBuffer += 4;
	}
}

//--------------------------------------------------------------------------------------

struct	ConstantBuffer
{
	glm::mat4	WorldViewProj;
};

//--------------------------------------------------------------------------------------

HRESULT	ParticleRendererBillboard::_ResizeBuffers(u32 newParticleCount)
{
	u32	newVertexCount = newParticleCount * 4;

	if (newVertexCount <= m_AvailableVertices)
		return S_OK;

	if (_buffersCreated)
	{
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ibo);
	}

	m_AvailableVertices = newVertexCount + (newVertexCount >> 1);
	u32	availableParticles = m_AvailableVertices / 4;
	u32	availableVertices = availableParticles * 4;	// quads
	u32	availableIndices = availableParticles * 6;	// 2 triangles per quad, 3 indices per triangle

	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ibo);

	u32*	dstIndices = new u32[availableIndices];
	int offset = 0;
	for (u32 i = 0; i < availableVertices; i += 4)
	{
		dstIndices[offset + 0] = i + 0;
		dstIndices[offset + 2] = i + 1;
		dstIndices[offset + 1] = i + 2;
		dstIndices[offset + 3] = i + 2;
		dstIndices[offset + 5] = i + 3;
		dstIndices[offset + 4] = i + 0;
		offset += 6;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * availableIndices, dstIndices, GL_STATIC_DRAW);
	delete[] dstIndices;
	return S_OK;
}

//--------------------------------------------------------------------------------------

void	ParticleRendererBillboard::_DestroyComponents()
{
	m_AvailableVertices = 0;
	if (_buffersCreated)
	{
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ibo);
	}
}

void	ParticleRendererBillboard::DestroyComponents()
{
	_DestroyComponents();
}

HRESULT	ParticleRendererBillboard::CreateComponents()
{
	return S_OK;
}

//--------------------------------------------------------------------------------------

void	ParticleRendererBillboard::Render(const ParticleStream &stream, const glm::mat4 &matWV, const glm::mat4 &matWVP, GLint attrCoord3D,
										  GLint attrVertexUV, GLint attrVertexNormal, GLint attrUniformModel, GLint attrTexture)
{
	// get the total particle count, so we know the VB size we need
	u32	totalParticleCount = stream.m_Count;
	if (totalParticleCount == 0)	// nothing to render
		return;

	glm::mat4	Model;
	Model = glm::translate(glm::mat4(), glm::vec3(0, 925, 0));

	glUniformMatrix4fv(attrUniformModel, 1, GL_FALSE, &Model[0][0]);

	// resize the vertex buffers
	if (FAILED(_ResizeBuffers(totalParticleCount)))
		return;
	assert(totalParticleCount <= m_AvailableVertices);

	Vertex*	dstVertex = new Vertex[totalParticleCount * 4];
	_FillVertexBuffer(stream, dstVertex, matWV);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * totalParticleCount * 4, dstVertex, GL_DYNAMIC_DRAW);
	delete[] dstVertex;

	glEnableVertexAttribArray(attrCoord3D);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(
		attrCoord3D,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(Vertex),      // stride
		(void*)0            // array buffer offset
	);
	
	glEnableVertexAttribArray(attrVertexNormal);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(
		attrVertexNormal,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(Vertex),      // stride
		(void*)12            // array buffer offset
	);
	
	glEnableVertexAttribArray(attrVertexUV);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(
	    attrVertexUV,                                // attribute. No particular reason for 1, but must match the layout in the shader.
	    2,                                // size
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized?
		sizeof(Vertex),						// stride
	    (void*)16                          // array buffer offset
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glDrawElements(GL_TRIANGLES, totalParticleCount * 2, GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(attrVertexUV);
	glDisableVertexAttribArray(attrVertexNormal);
	glDisableVertexAttribArray(attrCoord3D);
}

//--------------------------------------------------------------------------------------