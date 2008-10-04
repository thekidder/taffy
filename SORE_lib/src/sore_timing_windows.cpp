//$Id$

#include <windows.h>
#include <cassert>

#include "sore_timing.h"
#include "sore_logger.h"

namespace SORE_Timing
{
	unsigned int GetGlobalTicks() //in 1/10000 second increments
	{
		static unsigned __int64 start_count = 0;
		static unsigned __int64 freq        = 0;
		if(start_count == 0 && freq == 0)
		{
			QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
			QueryPerformanceCounter((LARGE_INTEGER*)&start_count);
		}
		unsigned __int64 ticks;
		QueryPerformanceCounter((LARGE_INTEGER*)&ticks);
		double secs = (double)(ticks - start_count) / (double)freq;
		return (unsigned int)(secs*10000.0);
		//return 10*SDL_GetTicks();
	}
	
	void Sleep(unsigned int ms)
	{
		::Sleep(ms);
	}
}