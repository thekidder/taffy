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

#include "sore_profiler.h"

#include "sore_allgl.h"

unsigned int SORE_Profiler::Sample::numSamples = 0;

namespace SORE_Profiler
{
	Sample::Sample(std::string name)
	{
		int currIndex = -1,i;
		for(i=0;i<MAX_SAMPLES;i++)
		{
			if(samples[i].name==name)
			{
				currIndex = i;
				break;
			}
		}
		if(currIndex<0)
		{
			index = numSamples++; 
			samples[index].name = name;
			samples[index].timesRun = 1;
			samples[index].startTime = SDL_GetTicks();
			lastOpened = index;
		}
		else
		{
			index = lastOpened = currIndex;
			samples[index].startTime = SDL_GetTicks();
			samples[index].timesRun++;
		}
	}
	
	Sample::~Sample()
	{
		samples[index].endTime = SDL_GetTicks();
		samples[index].avgTime = (samples[index].avgTime*samples[index].timesRun+(samples[index].endTime-samples[index].startTime))/++samples[index].timesRun;
	}
}
