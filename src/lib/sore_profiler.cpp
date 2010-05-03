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

#include "sore_logger.h"
#include "sore_profiler.h"
#include "sore_allgl.h"
#include "sore_timing.h"

unsigned int SORE_Profiler::Sample::numSamples = 0;
unsigned int SORE_Profiler::Sample::lastOpened = 0;
SORE_Profiler::Sample::SampleData SORE_Profiler::Sample::samples[SORE_Profiler::MAX_SAMPLES];

namespace SORE_Profiler
{
	Sample::Sample(std::string name)
	{
		int currIndex = GetSampleByName(name);
		
		if(currIndex<0)
		{
			index = numSamples++; 
			samples[index].name = name;
			samples[index].timesRun = 0;
			lastOpened = index;
			samples[index].intervalCount = 1;
		}
		else
		{
			index = lastOpened = currIndex;
			//samples[index].timesRun++;
			samples[index].intervalCount++;
		}
		samples[index].startTime = SORE_Timing::GetGlobalTicks();
	}
	
	Sample::~Sample()
	{
		samples[index].endTime = SORE_Timing::GetGlobalTicks();
		samples[index].lastTime = samples[index].endTime - samples[index].startTime;
		samples[index].avgTime = (samples[index].avgTime*samples[index].timesRun+(samples[index].lastTime))/(++samples[index].timesRun);
		samples[index].shortAvgTime+=samples[index].lastTime;
		if(samples[index].intervalCount%SHORT_INTERVAL==0)
		{
			samples[index].intervalCount = 0;
			samples[index].shortAvgTime/=SHORT_INTERVAL;
			//ENGINE_LOG(SORE_Logging::LVL_DEBUG1, "Average time for sample ID %d: %f", index, samples[index].shortAvgTime);
		}
	}
	
	int Sample::GetSampleByName(std::string name)
	{
		for(size_t i=0;i<MAX_SAMPLES;i++)
		{
			if(samples[i].name==name)
			{
				return static_cast<int>(i);
			}
		}
		return -1;
	}
	
	void Sample::DisplayAvgTime(int sID)
	{
		ENGINE_LOG(SORE_Logging::LVL_DEBUG1, boost::format("Average time for sample ID %d: %f ms") % sID % (samples[sID].avgTime / 10.0));
	}
	
	void Sample::DisplayTime(int sID)
	{
		ENGINE_LOG(SORE_Logging::LVL_DEBUG1, boost::format("Time for sample ID %d: %f ms") % sID % (samples[sID].lastTime / 10.0));
	}
}
