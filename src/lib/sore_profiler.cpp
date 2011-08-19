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

#include "sore_profiler.h"
#include "sore_timing.h"

#include <exception>

SORE_Profiler::Profiler::Profiler()
    : current(&data1), last(&data2)
{
    lastFrameStart = SORE_Kernel::GetGlobalMS();

    // initialize the root sample
    current->allSamples[""] = sample_data("");
    current->root = &current->allSamples[""];
    current->openSamples.push(&current->allSamples[""]);
}

void SORE_Profiler::Profiler::Frame(int elapsed)
{
    // update the root sample to finish this frame
    unsigned int frameEnd = SORE_Kernel::GetGlobalMS();
    double ms = static_cast<double>(frameEnd - lastFrameStart);
    UpdateSample(current->allSamples[""], ms);

    // swap the pointers
    profiler_data* temp = last;
    last = current;
    current = temp;

    // update current with last
    // (sets our new current frame to the actual last frame)
    *current = *last;
    current->root = &current->allSamples[""];

    // clear out the current frame
    for(Sample_map_t::iterator i = current->allSamples.begin(); i != current->allSamples.end(); ++i)
    {
        i->second.current = false;
        i->second.children.clear();
    }
    while(!current->openSamples.empty())
        current->openSamples.pop();
    current->openSamples.push(&current->allSamples[""]);

    lastFrameStart = SORE_Kernel::GetGlobalMS();
}

void SORE_Profiler::Profiler::StartSample(const Sample& sample)
{
    Sample_map_t::iterator it = current->allSamples.find(sample.name);
    if(it == current->allSamples.end())
    {
        // create a new sample: this one hasn't been seen before
        sample_data newSample(sample.name);
        it = current->allSamples.insert(std::make_pair(sample.name, newSample)).first;
    }
    current->openSamples.top()->children.push_back(&it->second);
    current->openSamples.push(&it->second);
}

void SORE_Profiler::Profiler::FinishSample(const Sample& sample)
{
    Sample_map_t::iterator it = current->allSamples.find(sample.name);
    if(it == current->allSamples.end())
    {
        // TODO: sore exceptions
        throw std::runtime_error("Unknown sample");
    }
    double ms = (sample.ticksEnd - sample.ticksStart) / 10.0;
    UpdateSample(it->second, ms);
}

const SORE_Profiler::sample_data* SORE_Profiler::Profiler::Samples() const
{
    return last->root;
}

void SORE_Profiler::Profiler::UpdateSample(sample_data& sample, double ms)
{
    sample.avgTime = (sample.avgTime * sample.timesRun + ms) / (sample.timesRun + 1);
    sample.lastTime = ms;
    sample.maxTime = std::max(sample.maxTime, ms);
    sample.minTime = std::min(sample.minTime, ms);

    ++sample.timesRun;
    sample.current = true;

    current->openSamples.pop();
}