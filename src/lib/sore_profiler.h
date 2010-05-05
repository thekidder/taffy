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
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
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

#ifndef  SORE_PROFILER_H
#define  SORE_PROFILER_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <string>

#include <sore_dll.h>

namespace SORE_Profiler
{
	const unsigned int MAX_SAMPLES = 20;
	const unsigned int SHORT_INTERVAL = 20;
	class SORE_EXPORT Sample
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
				double         startTime;
				double         endTime;
				unsigned int   timesRun;
				double         lastTime;
				
				double         avgTime;
				double         shortAvgTime;
				int            intervalCount;
				unsigned int   minTime;
				unsigned int   maxTime;
				std::string name;
				
			} samples[MAX_SAMPLES];
			static unsigned int numSamples;
			static unsigned int lastOpened;
		
	};
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
