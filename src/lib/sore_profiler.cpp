/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#include <sore_logger.h>
#include <sore_profiler.h>
#include <sore_allgl.h>
#include <sore_timing.h>

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
