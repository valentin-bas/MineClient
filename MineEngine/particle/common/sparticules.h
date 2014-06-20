#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_H__
#define	__SPARTICULES_H__

//----------------------------------------------------------------------------

#define	NOTHING
#define	COMMA			,
#define	GLUE(a, b)		__GLUE(a, b)
#define	STRINGIFY(s)	__STRINGIFY(s)

// don't directly use these...
#define	__GLUE(a, b)		a ## b
#define	__STRINGIFY(s)		# s

//----------------------------------------------------------------------------

#if	!defined(SP_RELEASE)
#	if	!defined(_DEBUG) || defined(NDEBUG)
#		define	SP_RELEASE
#	endif
#endif

#if	!defined(SP_DEBUG)
#	if	defined(_DEBUG) && !defined(NDEBUG)
#		define	SP_DEBUG
#	endif
#endif

//----------------------------------------------------------------------------
//
//	Operating-system detection
//
//----------------------------------------------------------------------------

// we need these to be undefined
#define	SP_OS_WINDOWS	0	// Windows, may actually be Win32 or Win64
#define	SP_OS_WINRT		1	// WindowsRT
#define	SP_OS_LINUX		2	// Linux
#define	SP_OS_MACOSX	3	// MacOSX

//----------------------------------------------------------------------------

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__win32__))

#	define	SP_WINDOWS	// defined for all windows-family OSes

#	if defined(WINAPI_FAMILY)
#		if (WINAPI_FAMILY == WINAPI_FAMILY_APP) || (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
#			define	SP_WINRT
#			define	SP_OS	SP_OS_WINRT
#		elif (WINAPI_FAMILY == WINAPI_PARTITION_DESKTOP) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#			define	SP_WIN32	// use this one, not 'WIN32'
#			define	SP_OS	SP_OS_WINDOWS
#		else
#			error	Unknown windows platform
#		endif
#	else
#		define	SP_WIN32	// use this one, not 'WIN32'
#		define	SP_OS		SP_OS_WINDOWS
#	endif

#elif (defined(LINUX) || defined(_LINUX) || defined(__LINUX__) || defined(__linux__))

#	define	SP_LINUX	// use this one, not 'LINUX'
#	define	SP_OS		SP_OS_LINUX

#elif (defined(MACOSX) || defined(__APPLE__) || defined(__apple__) || defined(macosx) || defined(MACOS_X))

#	define	SP_MACOSX	// use this one, not 'MACOSX' or '__APPLE__'
#	define	SP_OS		SP_OS_MACOSX

#else
#	error Unsupported platform
#endif

//----------------------------------------------------------------------------

#if		(SP_OS == SP_OS_WINDOWS)
#	define	SP_OS_NAME		"Windows"
#elif	(SP_OS == SP_OS_WINRT)
#	define	SP_OS_NAME		"WindowsRT"
#elif	(SP_OS == SP_OS_LINUX)
#	define	SP_OS_NAME		"Linux"
#elif	(SP_OS == SP_OS_MACOSX)
#	define	SP_OS_NAME		"MacOSX"
#else
#	error no SP_OS_NAME defined
#endif

//----------------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------------

#define	SP_CPU_X86			1
#define	SP_CPU_X64			2
#define	SP_CPU_IA64			3
#define	SP_CPU_ARM			4

#define SP_CPU_32BITS		32
#define SP_CPU_64BITS		64

//----------------------------------------------------------------------------

#if		defined(_MSC_VER)
#	define	SP_COMPILER_MSVC
#elif	defined(__ICL) || defined(__ICC) || defined(__INTEL_COMPILER)
#	define	SP_COMPILER_INTEL
#elif	defined(__clang__)
#	define	SP_COMPILER_CLANG
#	define	SP_COMPILER_GCC	// GCC-Compatible
#elif	defined(__GNUC__) || defined(__GNUG__)
#	define	SP_COMPILER_GCC
#elif	defined(__MWERKS__) || defined(__CWCC__)
#	define	SP_COMPILER_METROWERKS
#else
#	error	unknown compiler
#endif

//----------------------------------------------------------------------------

#if	!defined(SP_COMPILER_MSVC)	// currently only visual studio is supported
#	error	unsupported compiler
#endif

//----------------------------------------------------------------------------
//
//	MICROSOFT VISUAL STUDIO - VC++
//
//----------------------------------------------------------------------------

#define	SP_COMPILER_VERSION			_MSC_VER
#define	SP_COMPILER_VERSION_STRING	STRINGIFY(_MSC_VER)
#define	SP_COMPILER_NAME			"Microsoft Visual C++"

#ifdef _CHAR_UNSIGNED
#	error	bad compiler settings, chars must be signed...
#endif

// disable all this deprecated crap...
#ifndef	_CRT_SECURE_NO_DEPRECATE
#	define _CRT_SECURE_NO_DEPRECATE
#endif

#ifndef STRSAFE_NO_DEPRECATE
#	define STRSAFE_NO_DEPRECATE
#endif

#ifdef	_CPPRTTI		// has RTTI enabled in the projects settings
#	define	SP_USE_RTTI
#endif
#ifdef	_CPPUNWIND		// has exception handling enabled in the projects settings
#	define	SP_USE_EXCEPTIONS
#endif

#if	defined(_M_IX86) || defined(i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)
#	define	SP_CPU		SP_CPU_X86
#elif	defined(_M_AMD64) || defined(_M_X64) || defined(_WIN64)
#	define	SP_CPU		SP_CPU_X64
#elif	defined(_M_IA64) || defined(_IA64_)
#	define	SP_CPU		SP_CPU_IA64
#elif	(_XBOX_VER >= 200)
#	define	SP_CPU		SP_CPU_XENON
#elif	defined(_M_PPC) || defined(POWERPC)
#	define	SP_CPU		SP_CPU_PPC
#elif	defined(_M_ARM)
#	define	SP_CPU		SP_CPU_ARM
#else
#	error	unknown CPU architecture !
#endif

#if	defined(SP_WINDOWS)
#	if	!defined(SP_CPU_IA64)
#		define	SP_HAS_STDCALL
#	endif
#endif

#if	(SP_CPU == SP_CPU_ARM)
#	define	SP_HAS_ALLOCA		0
#else
#	define	SP_HAS_ALLOCA		1
#	define SP_ALLOCA(__size)	alloca(__size)
#endif

#ifndef	SP_HAS_TLS	// we can override this and force it off
#	define	SP_HAS_TLS			1
#endif

#if defined( _UNICODE ) || defined( UNICODE )
#	define	SP_UNICODE
#endif

// inlining helpers
#if	(_MSC_VER >= 1300)
#	define	SP_INLINE					__inline
#	define	SP_FORCEINLINE				__forceinline
#	define	SP_NOINLINE					__declspec(noinline)
#else
#	define	SP_INLINE					__inline
#	define	SP_FORCEINLINE				__inline
#	define	SP_NOINLINE
#endif

#define	SP_ASSUME(__expr)				__assume(__expr)
#ifdef	SP_DEBUG
#	define	SP_ASSUME_NOT_REACHED()		assert(false)
#else
#	define	SP_ASSUME_NOT_REACHED()		__assume(false)
#endif

#define	SP_NOALIAS						__declspec(noalias)
#define	SP_NOINLINE						__declspec(noinline)
#define	SP_FASTCALL						__fastcall
#define	SP_NOVTABLE						__declspec(novtable)
#define	SP_ALIGN(__a)					__declspec(align(__a))
#define	SP_ALIGNOF(__t)					__alignof(__t)

#define	SP_BREAKPOINT()					__debugbreak()	// __asm int 3

//----------------------------------------------------------------------------
// unwanted level 4 warnings to disable.
// warning C4201 : nonstandard extension used : nameless struct/union
// warning C4239 : nonstandard extension used : 'extern' before template explicit instantiation
// warning C4481 : nonstandard extension used: override specifier 'override'
// warning C4244 : conversion from 'int' to 'short', possible loss of data
// warning C4127 : Conditional expression is constant
// warning C4152 : nonstandard extension, function/data pointer conversion in expression
// warning C4231 : nonstandard extension used : 'identifier' before template explicit instantiation

// warning C4251 : needs to have dll-interface to be used by clients of ''
// warning C4275 : non dll-interface struct/class '' used as base for dll-interface struct/class ''
// warning C4355 : 'this' : used in base member initializer list

#pragma warning(disable : 4201 4239 4481 4244 4127 4152 4231)
#pragma warning(disable : 4251 4275 4355)	// dll-export interfaces

#if	!defined(SP_WARNINGS_PARANOID)
	// warning C4100 : '%s' unreferenced formal parameter
	// warning C4706 : assignment within conditional expression
	// warning C4701 : local variable '%s' may be used without having been initialized (usually, we wouldn't want that warning disabled, but the uninitialized variables detection in level 4 warnings is just plain dumb...)
	// warning C4710 : inline function not expanded
	// warning C4714 : function marked as __forceinline not inlined
	// warning C4206 : nonstandard extension used : translation unit is empty
	// warning C4057 : '%s' : '%t' differs in indirection to slightly different base types from '%t'
	// warning C4054 : 'type cast' : from function pointer '%t' to data pointer '%t'
	// warning C4055 : 'type cast' : from data pointer '%t' to function pointer '%t'

#	pragma warning(disable : 4100 4706 4701 4710 4714 4206 4057 4054 4055)

	// X64 compatibility warnings:
#	pragma warning(disable : 4312) // 'type cast' : conversion from '%t' to '%p' of greater size
#endif

//----------------------------------------------------------------------------
// warnings we want to treat as errors:

// warning C4002: too many actual parameters for macro '%s'
// warning C4003: not enough actual parameters for macro '%s'

#pragma warning(error : 4002 4003)

// C++0x features

#if	(_MSC_VER >= 1600)	// if greater or equal to Visual Studio 2010
#	define	SP_HAS_NULLPTR			nullptr
#	define	SP_HAS_STATIC_ASSERT
#	define	SP_HAS_VARIADIC_MACROS
#	ifndef	SP_HAS_AUTO
#		define	SP_HAS_AUTO			1
#	endif
/*#elif	(SP_CPU == SP_CPU_XENON)	// VC++ compiler for XBox360 has variadic macros (NOTE: in fact, its _MSC_VER appears to be more recent, so it correctly falls into the #if above
#	define	SP_HAS_VARIADIC_MACROS*/
#endif

// in release, force disabling of frame-pointer omission, to allow the callstacks to work correctly everywhere.
//#if	defined(SP_RELEASE) && (SP_CPU == SP_CPU_X86)
//#	pragma	optimize("y", off)
//#endif

//----------------------------------------------------------------------------

#define	SP_MESSAGE(__msg)		__pragma(message(__msg))
#define	SP_TODO(__msg)			__pragma( message(__FILE__ "(" STRINGIFY(__LINE__) ") /!\\/!\\/!\\/!\\ TODO /!\\/!\\/!\\/!\\ " __msg) )
#define	SP_FIXME(__msg)			__pragma( message(__FILE__ "(" STRINGIFY(__LINE__) ") /!\\/!\\/!\\/!\\ FIXME /!\\/!\\/!\\/!\\ " __msg) )
#define	SP_NOTE(__msg)			__pragma( message(__FILE__ "(" STRINGIFY(__LINE__) ") /!\\/!\\/!\\/!\\ NOTE /!\\/!\\/!\\/!\\ " __msg) )

//----------------------------------------------------------------------------
//	Check if everything went right...

#if	!defined(SP_CPU)
#	error	unsupported CPU architecture...
#endif

//----------------------------------------------------------------------------
//
//	OS
//
//----------------------------------------------------------------------------


#if !defined(_CRT_SECURE_NO_WARNINGS)
#	define	_CRT_SECURE_NO_WARNINGS
#endif
#if !defined(_CRT_SECURE_NO_DEPRECATE)
#	define	_CRT_SECURE_NO_DEPRECATE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef	POINTER_64
#	define POINTER_64	__ptr64
#endif

#ifndef	_WINDOWS_	// don't re-include windows.h if it was already manually included

#	ifndef	WIN32_LEAN_AND_MEAN
#		define	WIN32_LEAN_AND_MEAN			1
#		define	SP_WIN32_LEAN_AND_MEAN
#	endif
#	ifndef	WIN32_EXTRA_LEAN
#		define	WIN32_EXTRA_LEAN
#		define	SP_WIN32_EXTRA_LEAN
#	endif
#	ifndef	VC_EXTRALEAN
#		define	VC_EXTRALEAN
#		define	SP_VC_EXTRALEAN
#	endif

// these should already be ignored by VC_EXTRALEAN
#	if	!defined(SP_WINRT)

#		define	NOSERVICE
#		define	NOMCX
#		define	NOIME
#		define	NOSOUND
#		define	NOCOMM
#		define	NOKANJI
#		define	NORPC
#		define	NOPROXYSTUB
#		define	NOIMAGE
#		define	NOTAPE

// additional trims
#		define	NOGDICAPMASKS
#		define	NOVIRTUALKEYCODES
#		define	NOWINMESSAGES
#		define	NOWINSTYLES
#		define	NOSYSMETRICS
#		define	NOMENUS
#		define	NOICONS
#		define	NOKEYSTATES
#		define	NOSYSCOMMANDS
#		define	NORASTEROPS
#		define	NOSHOWWINDOW
#		define	OEMRESOURCE
#		define	NOATOM
#		define	NOCLIPBOARD
#		define	NOCOLOR
#		define	NOCTLMGR
#		define	NODRAWTEXT
#		define	NOGDI
#		define	NOKERNEL
#		define	NOUSER
#		define	NONLS
#		define	NOMB
#		define	NOMEMMGR
#		define	NOMETAFILE
#		define	NOMINMAX
#		define	NOMSG
#		define	NOOPENFILE
#		define	NOSCROLL
#		define	NOTEXTMETRIC
#		define	NOWH
#		define	NOWINOFFSETS
#		define	NOHELP
#		define	NOPROFILER
#		define	NODEFERWINDOWPOS

#	endif	// !defined(SP_WINRT)

#	include	<windows.h>

#	ifdef	SP_WIN32_LEAN_AND_MEAN
#		undef	WIN32_LEAN_AND_MEAN
#		undef	SP_WIN32_LEAN_AND_MEAN
#	endif
#	ifdef	SP_WIN32_EXTRA_LEAN
#		undef	WIN32_EXTRA_LEAN
#		undef	SP_WIN32_EXTRA_LEAN
#	endif
#	ifdef	SP_VC_EXTRALEAN
#		undef	VC_EXTRALEAN
#		undef	SP_VC_EXTRALEAN
#	endif

#endif	// _WINDOWS_

#ifdef	SP_COMPILER_MSVC
#	undef	strcasecmp
#	undef	strncasecmp
#	define	strcasecmp	_stricmp
#	define	strncasecmp	_strnicmp
#endif


//--------------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------------

// platform intrinsics:
#if		(SP_CPU == SP_CPU_X86) || (SP_CPU == SP_CPU_X64) || (SP_CPU == SP_CPU_IA64)
#	if	!defined(SP_COMPILER_GCC) || defined(__SSE__)
#		include <xmmintrin.h>
#	endif
#endif

//----------------------------------------------------------------------------

#if		defined(SP_DEBUG)
#	define SP_ONLY_IN_DEBUG(__expr)			__expr
#	define SP_ONLY_IN_RELEASE(__expr)		NOTHING
#else
#	define SP_ONLY_IN_DEBUG(__expr)			NOTHING
#	define SP_ONLY_IN_RELEASE(__expr)		__expr
#endif

#define	SP_ARRAY_COUNT(__arr)				(sizeof(__arr) / sizeof((__arr)[0]))

//--------------------------------------------------------------------------------------

template<typename _Type>
_Type	clamp(_Type x, _Type min, _Type max)
{
	return x > min ? x < max ? x : max : min;
}

template<typename _Type, typename _TypeFrac>
_Type	lerp(const _Type a, const _Type b, const _TypeFrac f)
{
	return a + f * (b - a);
}

template<typename _Type>
SP_FORCEINLINE _Type	sign(_Type x)
{
	return x < _Type(0) ? _Type(-1) : _Type(1);
}

template<typename _Type>
SP_FORCEINLINE _Type	squared(_Type value)
{
	return value * value;
}

//----------------------------------------------------------------------------
// use to cast, checking that there is no loss of information

template<typename _TypeDst, typename _TypeSrc>
SP_FORCEINLINE _TypeDst	checked_numcast(const _TypeSrc &value)
{
	_TypeDst	result = static_cast<_TypeDst>(value);
	assert(static_cast<_TypeSrc>(result) == value);
	return result;
}

//----------------------------------------------------------------------------
// bit-level cast (ex: replaces *((int*)&floatValue), without messing up pointer aliasing)

template<typename _TypeDst, typename _TypeSrc>
SP_FORCEINLINE _TypeDst	bit_cast(const _TypeSrc &value)
{
	static_assert(sizeof(_TypeDst) == sizeof(_TypeSrc), "mismatching sizes for bit_cast<>()");
	union
	{
		_TypeSrc	a;
		_TypeDst	b;
	}	caster;
	caster.a = value;
	return caster.b;
}

//--------------------------------------------------------------------------------------
// NOTE: EXTREMELY BAD RAND FUNCTION ! don't use that. this is only for the sample's sake.

SP_FORCEINLINE float	randfp(float min, float max)	// [min, max] range
{
	assert(max >= min);
	float	range = max - min;
	float	fp32_01 = rand() / float(RAND_MAX);	// [0,1] range
	return fp32_01 * range + min;
}

//----------------------------------------------------------------------------

#endif	// __SPARTICULES_H__
