//
// C++ Interface: sore_profiler
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
// $Id$


#ifndef  __SORE_PROFILER_H__
#define  __SORE_PROFILER_H__

#include <string>

namespace SORE_Profiler
{
	const unsigned int MAX_SAMPLES = 20;
	const unsigned int SHORT_INTERVAL = 20;
	class Sample
	{
		public:
			Sample(std::string name);
			~Sample();
			static int GetSampleByName(std::string name);
			static void DisplayAvgTime(int sID);
			static void DisplayTime(int sID);
		protected:
			
			unsigned int index;
			
			static struct SampleData
			{
				SampleData()
				{
					startTime = endTime = 0;
					minTime = maxTime = 0;
					avgTime = 0.0;
					intervalCount = 0;
				}
				unsigned int   startTime;
				unsigned int   endTime;
				unsigned int   timesRun;
				unsigned int   lastTime;
				
				         float avgTime;
								 float shortAvgTime;
				         int   intervalCount;
				unsigned int   minTime;
				unsigned int   maxTime;
				std::string name;
				
			} samples[MAX_SAMPLES];
			static unsigned int numSamples;
			static unsigned int lastOpened;
		
	};
}

#endif /*__SORE_PROFILER_H__*/
