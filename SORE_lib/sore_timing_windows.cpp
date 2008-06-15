//$Id$

#include "sore_timing.h"
#include <windows.h>

namespace SORE_Timing
{
	void Sleep(unsigned int ms)
	{
		::Sleep(ms);
	}
}