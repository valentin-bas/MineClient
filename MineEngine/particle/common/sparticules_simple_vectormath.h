#pragma once
//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_SIMPLE_VECTORMATH_H__
#define	__SPARTICULES_SIMPLE_VECTORMATH_H__

//----------------------------------------------------------------------------

static SP_INLINE float3	operator+ (const float3 &rhs, const float3 &lhs)
{
	return float3(	rhs.x + lhs.x,
					rhs.y + lhs.y,
					rhs.z + lhs.z);
}

//----------------------------------------------------------------------------

static SP_INLINE float3	operator- (const float3 &rhs, const float3 &lhs)
{
	return float3(	rhs.x - lhs.x,
					rhs.y - lhs.y,
					rhs.z - lhs.z);
}

//----------------------------------------------------------------------------

static SP_INLINE float3	operator* (const float3 &rhs, float lhs)
{
	return float3(	rhs.x * lhs,
					rhs.y * lhs,
					rhs.z * lhs);
}

//----------------------------------------------------------------------------

static SP_INLINE float3	operator* (float rhs, const float3 &lhs)
{
	return float3(	rhs * lhs.x,
					rhs * lhs.y,
					rhs * lhs.z);
}

//----------------------------------------------------------------------------

static SP_INLINE float	dot(const float3 &a, const float3 &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

//----------------------------------------------------------------------------

static SP_INLINE float	dot(const float4 &a, const float4 &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

//----------------------------------------------------------------------------

#endif	// __SPARTICULES_SIMPLE_VECTORMATH_H__
