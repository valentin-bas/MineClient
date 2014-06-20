#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_TIMERS_INL__
#define	__SPARTICULES_TIMERS_INL__

#ifndef	__SPARTICULES_TIMERS_H__
#	error	sparticules_timers.inl must not be included manually, include sparticules_timers.h instead
#endif

#if	defined(SP_WINDOWS)
#	include <intrin.h>
#else
#	error	unsupported Timers API
#endif

//----------------------------------------------------------------------------
//
//	Windows implementation
//
//----------------------------------------------------------------------------

#if	defined(SP_WINDOWS)

//----------------------------------------------------------------------------

template<typename _Type>
class	TBaseTimer
{
private:
	enum	EFlags
	{
		Flags_Started = 0x80,
	};

	u32				m_Flags;
	mutable double	m_Frequency;
	u64				m_St;

	static_assert(sizeof(LARGE_INTEGER) == sizeof(u64), "invalid build configuration");
	SP_FORCEINLINE static double	_InternalFrequencySample()
	{
		u64 lfreq;
		::QueryPerformanceFrequency((LARGE_INTEGER*)&lfreq);
		if (lfreq != 0)
			return 1.0 / lfreq;
		assert(false);	// not reached
		return 1e-6;
	}

protected:
	SP_FORCEINLINE void				_InternalConfigure(u32 flags) { m_Flags = (m_Flags & Flags_Started) | (flags & ~Flags_Started); }
	SP_FORCEINLINE static u64		_InternalSample() { u64 counter; ::QueryPerformanceCounter((LARGE_INTEGER*)&counter); return counter; }
	SP_FORCEINLINE void				_InternalStart() { m_Flags |= Flags_Started; m_St = _InternalSample(); }
	SP_FORCEINLINE _Type			_InternalStop() { _Type t = _InternalRead(); m_Flags &= ~Flags_Started; return t; }
	_Type							_InternalRead() const
	{
		if (!(m_Flags & Flags_Started))
			return 0;

		u64	lint = _InternalSample();	// do this first so we may minimize the extra-time due to our internals computations...
		assert(m_Frequency != 0.0);
		if (m_Flags & SP_TIMERS_ALWAYS_RESYNC)
			m_Frequency = _InternalFrequencySample();

#ifdef	SP_TIMERS_SAFE_TIMERS
		// may be true in some (very) rare cases, when calling timer reads very frequently in very small timesteps...
		if (m_St > lint)
			lint = m_St + 1;
#endif
		return _Type(m_Frequency * (double)(lint - m_St));
	}

public:
	TBaseTimer()
	:	m_Flags(0)
	,	m_St(0)
	,	m_Frequency(_InternalFrequencySample())
	{
	}
	~TBaseTimer() {}
};

//----------------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------------

template<typename _TCycles, typename _TSignedCycles>
SP_FORCEINLINE _TCycles	TTimerCPUCycles<_TCycles, _TSignedCycles>::RawCPUCycles(bool serializeCPU /*= false*/)
{
#if	(SP_CPU == SP_CPU_X86 || SP_CPU == SP_CPU_X64)
	if (serializeCPU)
	{
		int	dummy[4];
		__cpuid(dummy, 0);
	}
	return __rdtsc();
#else
	//return __mftb();	// this has a known problem, see the Xbox360 documentation
	LARGE_INTEGER	ticks;
	QueryPerformanceCounter(&ticks);
	return ticks.QuadPart;
#endif
}

//----------------------------------------------------------------------------

template<typename _TCycles, typename _TSignedCycles>
SP_FORCEINLINE _TCycles	TTimerCPUCycles<_TCycles, _TSignedCycles>::RawCPUCyclesToRealCycles(_TCycles cycles)
{
	return cycles;
}

#endif	// defined(SP_WINDOWS)

//----------------------------------------------------------------------------
//
//	Generic common impl
//
//----------------------------------------------------------------------------

namespace	Timer
{
	namespace	Internal
	{
		extern u32		g_TimerCPUCyclesOverhead[2];	// [0] unserialized, [1] serialized. index directly with the 'serializeCPU' parameter

		void			Startup();						// call once at initialization to startup timers
	}
}

//----------------------------------------------------------------------------

template<typename _TCycles, typename _TSignedCycles>
TTimerCPUCycles<_TCycles, _TSignedCycles>::TTimerCPUCycles()
:	m_CyclesStart(0)
,	m_CyclesEnd(0)
{
}

//----------------------------------------------------------------------------

template<typename _TCycles, typename _TSignedCycles>
SP_FORCEINLINE void		TTimerCPUCycles<_TCycles, _TSignedCycles>::Start(bool serializeCPU /*= false*/)
{
	m_CyclesEnd = 0;
	m_CyclesStart = RawCPUCycles(serializeCPU);
}

//----------------------------------------------------------------------------

template<typename _TCycles, typename _TSignedCycles>
SP_FORCEINLINE _TCycles	TTimerCPUCycles<_TCycles, _TSignedCycles>::Stop(bool serializeCPU /*= false*/)
{
	_TSignedCycles	cycles = RawCPUCycles(serializeCPU);
	if (m_CyclesEnd == 0)	// not stopped yet
	{
		assert(u32(serializeCPU) == 0 || u32(serializeCPU) == 1);	// unfortunately, the standard doesn't guarantee anything
		m_CyclesEnd = cycles - Timer::Internal::g_TimerCPUCyclesOverhead[serializeCPU];
	}
	cycles = m_CyclesEnd - m_CyclesStart;

	// as this is meant to be used to profile stuff that take quite some time,
	// the thread may have been rescheduled into a core that wasn't synchronized with the original one.
	// make sure we return something not totally meaningless:
	return RawCPUCyclesToRealCycles((cycles >= 1) ? cycles : 1);
}

//----------------------------------------------------------------------------

template<typename _TCycles, typename _TSignedCycles>
SP_FORCEINLINE _TCycles	TTimerCPUCycles<_TCycles, _TSignedCycles>::Read(bool serializeCPU /*= false*/) const
{
	_TSignedCycles	cycles = m_CyclesEnd;
	if (cycles == 0)
	{
		assert(u32(serializeCPU) == 0 || u32(serializeCPU) == 1);	// unfortunately, the standard doesn't guarantee anything
		cycles = RawCPUCycles(serializeCPU) - Timer::Internal::g_TimerCPUCyclesOverhead[serializeCPU];
	}
	cycles -= m_CyclesStart;
	return RawCPUCyclesToRealCycles((cycles >= 1) ? cycles : 1);
}

//----------------------------------------------------------------------------

template<typename _TCycles, typename _TSignedCycles>
SP_FORCEINLINE _TCycles	TTimerCPUCycles<_TCycles, _TSignedCycles>::RawProcessorTimerOverhead(bool serializeCPU /*= false*/)
{
	volatile _TCycles	a = RawCPUCycles(serializeCPU);
	volatile _TCycles	b = RawCPUCycles(serializeCPU);
	return b - a;
}

//----------------------------------------------------------------------------

#endif // __KR_TIMERS_INL__

