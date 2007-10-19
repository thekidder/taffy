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
//

#ifndef  __SORE_PROFILER_H__
#define  __SORE_PROFILER_H__

namespace SORE_Profiler
{
	const unsigned int MAX_SAMPLES = 20;
	class Sample
	{
		public:
			Sample(char* name);
		protected:
			
			static struct SampleData
			{
				SampleData()
				{
					startTime = endTime = 0;
					minTime = maxTime = 0;
					avgTime = 0.0;
				}
				unsigned int   startTime;
				unsigned int   endTime;
				
				         float avgTime;
				unsigned int   minTime;
				unsigned int   maxTime;
				char name[20];
				
				unsigned int lastOpened;
			} samples[MAX_SAMPLES];
	};
}

#endif /*__SORE_PROFILER_H__*/