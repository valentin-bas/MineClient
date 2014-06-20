#pragma once
//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_TYPES_H__
#define	__SPARTICULES_TYPES_H__

//----------------------------------------------------------------------------
// basic types

#if	defined(SP_COMPILER_MSVC)

typedef	unsigned __int64	u64;
typedef	unsigned __int32	u32;
typedef	unsigned __int16	u16;
typedef	unsigned __int8		u8;
typedef	signed __int64		s64;
typedef	signed __int32		s32;
typedef	signed __int16		s16;
typedef	signed __int8		s8;

#else
#	error	Unsupported compiler
#endif

//----------------------------------------------------------------------------
// check our types have the expected size:

static_assert(sizeof(u8)		== 1, "unexpected sizeof(u8)");
static_assert(sizeof(u16)		== 2, "unexpected sizeof(u16)");
static_assert(sizeof(u32)		== 4, "unexpected sizeof(u32)");
static_assert(sizeof(u64)		== 8, "unexpected sizeof(u64)");
//static_assert(sizeof(ureg)		== sizeof(void*), "unexpected sizeof(ureg)");

static_assert(sizeof(s8)		== 1, "unexpected sizeof(s8)");
static_assert(sizeof(s16)		== 2, "unexpected sizeof(s16)");
static_assert(sizeof(s32)		== 4, "unexpected sizeof(s32)");
static_assert(sizeof(s64)		== 8, "unexpected sizeof(s64)");
//static_assert(sizeof(sreg)		== sizeof(void*), "unexpected sizeof(sreg)");

static_assert(sizeof(float)		== 4, "unexpected sizeof(float)");
static_assert(sizeof(double)	== 8, "unexpected sizeof(double)");

//--------------------------------------------------------------------------------------

struct	float2
{
	float	x;
	float	y;

	float2() { }
	float2(float _x, float _y) : x(_x), y(_y) { }
};
static_assert(sizeof(float2)		== 8, "unexpected sizeof(float2)");

//--------------------------------------------------------------------------------------

struct	float3
{
	float	x;
	float	y;
	float	z;

	float3() { }
	float3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }
};
static_assert(sizeof(float3)		== 12, "unexpected sizeof(float3)");

//--------------------------------------------------------------------------------------

struct	float4
{
	float	x;
	float	y;
	float	z;
	float	w;

	float4() { }
	float4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }
	float4(const float3 &v, float _w) : x(v.x), y(v.y), z(v.z), w(_w) { }
};
static_assert(sizeof(float4)		== 16, "unexpected sizeof(float4)");

//----------------------------------------------------------------------------

#endif	// __SPARTICULES_TYPES_H__
