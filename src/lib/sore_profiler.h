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
