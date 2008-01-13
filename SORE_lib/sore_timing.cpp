
#include "sore_timing.h"
#include "sore_allgl.h"
#include "sore_logger.h"

#ifndef WIN32
#include <sys/time.h>
#else
#include <windows.h>
#endif
#include <cassert>

namespace SORE_Timing
{
	unsigned int GetGlobalTicks() //in 1/10000 second increments
	{
#ifndef WIN32
		static unsigned long start_s = 0;
		static long start_us = 0;
		
		timeval tv;
		gettimeofday(&tv, NULL);
				
		if(start_s == 0 && start_us==0)
		{
			start_s  = tv.tv_sec;
			start_us = tv.tv_usec;
		}
		unsigned int nano_s  = 10000*(tv.tv_sec-start_s);
		int          nano_us = (tv.tv_usec-start_us)/100;
		unsigned int time = nano_s + nano_us;
		if(time<0)
		{
			ENGINE_LOG(SORE_Logging::LVL_CRITICAL, boost::format("Global ticks count is less than zero (ticks=%s)") % time);
		}
		return time;
#else
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
#endif
	}
}
