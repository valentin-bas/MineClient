//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "sparticules_updater_color.h"

//--------------------------------------------------------------------------------------

ParticleUpdaterColor::ParticleUpdaterColor(u32 colorStart, u32 colorEnd)
{
	// extract B G R A channels and precompute deltas for quicker update iterations
	for (u32 i = 0; i < 4; i++)
	{
		u32	channelStart = (colorStart & 0xFF);
		u32	channelEnd   = (colorEnd & 0xFF);

		m_ColorStart[i] = s16(channelStart) << 8;
		m_ColorDelta[i] = s16(channelEnd) - s16(channelStart);

		colorEnd >>= 8;
		colorStart >>= 8;
	}
}

//--------------------------------------------------------------------------------------

void	ParticleUpdaterColor::Update(ParticleStream &stream, float dt) const
{
	for (u32 i = 0, stop = stream.m_Count; i < stop; i++)
	{
		float	t = stream.m_LifeRatios[i];

		s16		t16 = t * 0x100;

		u32	b = u16(m_ColorStart[0] + t16 * m_ColorDelta[0]) >> 8;
		u32	g = u16(m_ColorStart[1] + t16 * m_ColorDelta[1]) >> 8;
		u32	r = u16(m_ColorStart[2] + t16 * m_ColorDelta[2]) >> 8;
		u32	a = u16(m_ColorStart[3] + t16 * m_ColorDelta[3]) >> 8;

		u32	newColor = b | (g << 8) | (r << 16) | (a << 24);
		stream.m_Colors[i] = newColor;
	}
}

//--------------------------------------------------------------------------------------

