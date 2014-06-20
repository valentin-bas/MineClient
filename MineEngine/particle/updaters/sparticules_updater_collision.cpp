//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "../common/sparticules_simple_vectormath.h"
#include "sparticules_updater_collision.h"

//--------------------------------------------------------------------------------------

ParticleUpdaterCollision::ParticleUpdaterCollision(const float3 &planeNormal, float planeDistanceToOriginAlongNormal, float restitution)
:	m_PlaneEquation(planeNormal.x,
					planeNormal.y,
					planeNormal.z,
					planeDistanceToOriginAlongNormal)
,	m_Restitution(restitution)
{
}

//--------------------------------------------------------------------------------------

#define	COLL_EPSILON	1.0e-5f

void	ParticleUpdaterCollision::Update(ParticleStream &stream, float dt) const
{
	float3	planeNormal = float3(m_PlaneEquation.x, m_PlaneEquation.y, m_PlaneEquation.z);

	for (u32 i = 0, stop = stream.m_Count; i < stop; i++)
	{
		const float3	&sp = stream.m_PrevPositions[i];	// start pos
		float3			&ep = stream.m_Positions[i];		// end pos

		float4			hpStart	= float4(sp, 1.0f);	// build homogeneous position coordinate.
		float4			hpEnd	= float4(ep, 1.0f);	// build homogeneous position coordinate.

		float	dEnd = dot(hpEnd, m_PlaneEquation);
		if (dEnd < 0)	// if ending position is on the other side of the plane => collision
		{
			float	dStart = dot(hpStart, m_PlaneEquation);
			if (dStart >= 0)	// should always be true. otherwise means the particle has started under the plane. but can still happen, so check it anyway
			{
				float	dTotal = dStart - dEnd;
				float	moveFractionBeforeHit = dStart / dTotal;

				assert(moveFractionBeforeHit >= 0.0f && moveFractionBeforeHit <= 1.0f);

				// compute hit point:
				float3	startToEnd = ep - sp;
				float3	startToHit = startToEnd * moveFractionBeforeHit;

				float3	hitPoint = sp + startToHit;
				float3	hitNormal = planeNormal;

				// collision response:

				// reflect the end of the movement vector along the plane normal:
				float3	hitToEnd = startToEnd - startToHit;
				float3	hitToReflectedEnd = hitToEnd - hitNormal * (2 * dot(hitToEnd, hitNormal) - COLL_EPSILON);
				ep = hitPoint + m_Restitution * hitToReflectedEnd;

				// reflect velocity along the plane normal:
				float3	&v = stream.m_Velocities[i];
				float3	vReflected = v - hitNormal * (2 * dot(v, hitNormal));
				v = m_Restitution * vReflected;
			}
		}
	}
}

//--------------------------------------------------------------------------------------

