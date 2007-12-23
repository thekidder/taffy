
#include "sore_timing.h"
#include "sore_allgl.h"
#include "sore_logger.h"

#include <sys/time.h>
#include <cassert>

namespace SORE_Timing
{
	unsigned int GetGlobalTicks() //in 1/10000 second increments
	{
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
			ENGINE_LOG(SORE_Logging::LVL_CRITICAL, "Global ticks count is less than zero (ticks=%s)", time);
		}
		return time;
	}
}
