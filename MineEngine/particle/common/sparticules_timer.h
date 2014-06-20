#pragma once

//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#ifndef	__SPARTICULES_TIMERS_H__
#define	__SPARTICULES_TIMERS_H__

//----------------------------------------------------------------------------

// timers configuration flags
#define	SP_TIMERS_ALWAYS_RESYNC	0x1
#define	SP_TIMERS_SAFE_TIMERS

template<typename _Type> class	TBaseTimer;

//----------------------------------------------------------------------------

template<typename _Type>
class	TTimerSeconds : public TBaseTimer<_Type>
{
private:
	_Type	m_Accumulator;

protected:
public:
	TTimerSeconds() : m_Accumulator(0) {}
	~TTimerSeconds() {}

	SP_FORCEINLINE void			Configure(u32 flags) { TBaseTimer<_Type>::_InternalConfigure(flags); }
	SP_FORCEINLINE _Type		Read() const { return m_Accumulator + TBaseTimer<_Type>::_InternalRead(); }	// undefined results if the timer is stopped

	void						Start() { m_Accumulator = 0; TBaseTimer<_Type>::_InternalStart(); }
	_Type						Pause() { _Type duration = Stop(); m_Accumulator = duration; return duration; }
	_Type						Resume() { TBaseTimer<_Type>::_InternalStart(); return m_Accumulator; }
	void						Clear() { Start(); }

	_Type						Stop() { _Type duration = m_Accumulator + TBaseTimer<_Type>::_InternalStop(); m_Accumulator = 0; return duration; }
};

//----------------------------------------------------------------------------
//
//	CPU Cycles timer
//	typical overhead on a P4: ~63 cycles
//
//----------------------------------------------------------------------------

template<typename _TCycles, typename _TSignedCycles>
class	TTimerCPUCycles
{
private:
	_TCycles			m_CyclesStart;
	_TCycles			m_CyclesEnd;

public:
	TTimerCPUCycles();

	void				Start(bool serializeCPU = false);
	_TCycles			Stop(bool serializeCPU = false);						// returns real cycles
	_TCycles			Read(bool serializeCPU = false) const;					// returns real cycles
	static _TCycles		RawCPUCycles(bool serializeCPU = false);				// returns raw cycles
	static _TCycles		RawProcessorTimerOverhead(bool serializeCPU = false);	// returns raw cycles
	static _TCycles		RawCPUCyclesToRealCycles(_TCycles cycles);				// converts raw cycles to real cycles
};

//----------------------------------------------------------------------------
//
//	CPU Cycles timer
//	full 64 bits precision, larger object
//	typical overhead on a P4: ~63 cycles
//
//----------------------------------------------------------------------------
// 16 bytes

typedef TTimerCPUCycles<u64, s64>	TimerCycles;

//----------------------------------------------------------------------------

#include "sparticules_timer.inl"

//----------------------------------------------------------------------------

typedef TTimerSeconds<double>	TimerSeconds;

//----------------------------------------------------------------------------

#endif	// __SPARTICULES_TIMERS_H__
