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

#ifndef SORE_PROFILER_H
#define SORE_PROFILER_H

#include "sore_sample.h"
#include "sore_task.h"

#include <boost/unordered_map.hpp>

#include <deque>

namespace SORE_Profiler
{
    class Profiler : public SORE_Kernel::Task
    {
    public:
        Profiler();

        const char* GetName() const { return "Profiler task"; }
        void Frame(int elapsed);

        // usually called automatically by Sample constructor
        void StartSample(const Sample& sample);
        // usually called automatically by Sample destructor
        void FinishSample(const Sample& sample);

        // root sample data to analyze
        // this will be delayed one frame to provide complete information
        const sample_data* Samples() const;
    private:
        void UpdateSample(sample_data& sample, double ms);
        // the fully qualified sample name includes the entire call stack of the sample tree
        // this makes sure we create 2 samples for functions called in different places in the call stack
        std::string FullyQualifiedName(const Sample& sample);
        // return the FQN of the sample on the top of the stack
        std::string FullyQualifiedName();

        typedef boost::unordered_map<std::string, sample_data> Sample_map_t;
        struct profiler_data
        {
            
            Sample_map_t allSamples;
            sample_data* root;

            std::deque<sample_data*> openSamples;
        };

        unsigned int lastFrameStart;

        // use two copies of our state and flip-flop between them. That way we
        // always have a complete state from last frame we can give to consumers
        profiler_data data1, data2;

        profiler_data* current;
        profiler_data* last;
    };
}

#endif