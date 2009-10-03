/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


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
