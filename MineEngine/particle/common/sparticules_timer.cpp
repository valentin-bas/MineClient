//----------------------------------------------------------------------------
//
//	Sparticules 2 - ISART DIGITAL
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "sparticules_timer.h"

//----------------------------------------------------------------------------

namespace	Timer
{
	namespace	Internal
	{
		u32		g_TimerCPUCyclesOverhead[2] = { 0, 0 };

		void		Startup()						// call once at initialization to startup timers
		{
			// compute a average of the CPU overhead, over a large number of iterations:
			static const u32	kCPUCloclOverheadBenchIterations = 256;

			for (u32 k = 0; k < SP_ARRAY_COUNT(Timer::Internal::g_TimerCPUCyclesOverhead); k++)
			{
				u64	overheadAccum = 0;
				for (u32 i = 0; i < kCPUCloclOverheadBenchIterations; i++)
				{
					overheadAccum += TTimerCPUCycles<u64, s64>::RawProcessorTimerOverhead(k != 0);
				}

				Timer::Internal::g_TimerCPUCyclesOverhead[k] = overheadAccum / kCPUCloclOverheadBenchIterations;
			}
		}

	}
}

//----------------------------------------------------------------------------
