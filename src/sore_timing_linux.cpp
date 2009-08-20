/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// $Id$

#include <cassert>
#include <unistd.h>
#include <sys/time.h>

#include "sore_timing.h"
#include "sore_logger.h"

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
			ENGINE_LOG(SORE_Logging::LVL_CRITICAL, boost::format("Global ticks count is less than zero (ticks=%s)") % time);
		}
		return time;
	}
	
	void Sleep(unsigned int ms)
	{
		usleep(ms*1000);
	}
}
