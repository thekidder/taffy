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

#ifndef SORE_SAMPLE_H
#define SORE_SAMPLE_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <limits>
#include <string>
#include <vector>

namespace SORE_Profiler
{
	class Profiler;

    struct timing_sample
    {
        timing_sample() 
            : timesRun(0), 
            lastTime(0.0), totalTime(0.0),
            avgTime(0.0), minTime(std::numeric_limits<double>::max()), maxTime(0.0)
        {}

        unsigned int timesRun;

        // all times in ms
        double       lastTime;
        double       totalTime;

        double       avgTime;
        double       minTime;
        double       maxTime;
    };

    struct sample_data
    {
        sample_data(const std::string& name_ = "") 
            : name(name_), current(false)
        {}

        std::string name;
        std::vector<sample_data*> children;

        // has the sample been added this frame
        bool current; 

        timing_sample total;
        timing_sample last;
    };

    class Sample
    {
        public:
            Sample(const std::string& name_, Profiler& profiler_);
            ~Sample();

        private:
            friend class Profiler;

            Profiler& profiler;

            std::string name;

            // in 1/10000 s: default timer resolution
            unsigned int ticksStart;
            unsigned int ticksEnd;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
