//
// C++ Implementation: sore_profiler
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "sore_logger.h"
#include "sore_profiler.h"
#include "sore_allgl.h"

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
			samples[index].timesRun = 1;
			samples[index].startTime = SDL_GetTicks();
			lastOpened = index;
			samples[index].intervalCount = 1;
		}
		else
		{
			index = lastOpened = currIndex;
			samples[index].startTime = SDL_GetTicks();
			samples[index].timesRun++;
			samples[index].intervalCount++;
		}
	}
	
	Sample::~Sample()
	{
		samples[index].endTime = SDL_GetTicks();
		samples[index].lastTime = samples[index].endTime - samples[index].startTime;
		samples[index].avgTime = (samples[index].avgTime*samples[index].timesRun+(samples[index].lastTime))/++samples[index].timesRun;
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
		for(int i=0;i<MAX_SAMPLES;i++)
		{
			if(samples[i].name==name)
			{
				return i;
			}
		}
		return -1;
	}
	
	void Sample::DisplayAvgTime(int sID)
	{
		ENGINE_LOG(SORE_Logging::LVL_DEBUG1, "Average time for sample ID %d: %f", sID, samples[sID].avgTime);
	}
	
	void Sample::DisplayTime(int sID)
	{
		ENGINE_LOG(SORE_Logging::LVL_DEBUG1, "Time for sample ID %d: %d", sID, samples[sID].lastTime);
	}
}
