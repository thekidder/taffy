// $Id$

#ifndef  __SORE_TIMERS_H__
#define  __SORE_TIMERS_H__

namespace SORE_Timing
{
	unsigned int GetGlobalTicks(); // in 1/10000 sec
	
	void Sleep(unsigned int ms);
}

#endif /*__SORE_TIMERS_H__*/
