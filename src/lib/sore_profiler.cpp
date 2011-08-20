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
    current->allSamples["root"] = sample_data("root");
    current->root = &current->allSamples["root"];
    current->openSamples.push_back(&current->allSamples["root"]);
}

void SORE_Profiler::Profiler::Frame(int elapsed)
{
    // update the root sample to finish this frame
    unsigned int frameEnd = SORE_Kernel::GetGlobalMS();
    double ms = static_cast<double>(frameEnd - lastFrameStart);
    UpdateSample(current->allSamples["root"], ms);

    // update total frame time for all samples
    for(Sample_map_t::iterator i = current->allSamples.begin(); i != current->allSamples.end(); ++i)
    {
        i->second.total.lastTime = i->second.last.totalTime;
        i->second.total.totalTime += i->second.last.totalTime;

        i->second.total.avgTime = (i->second.total.avgTime * i->second.total.timesRun + i->second.last.totalTime) / (i->second.total.timesRun + 1);
        i->second.total.maxTime = std::max(i->second.total.maxTime, i->second.last.totalTime);
        i->second.total.minTime = std::min(i->second.total.minTime, i->second.last.totalTime);

        ++i->second.total.timesRun;
    }

    // swap the pointers
    profiler_data* temp = last;
    last = current;
    current = temp;

    // update current with last
    // (sets our new current frame to the actual last frame)
    *current = *last;
    current->root = &current->allSamples["root"];

    // clear out the current frame
    for(Sample_map_t::iterator i = current->allSamples.begin(); i != current->allSamples.end(); ++i)
    {
        i->second.current = false;
        i->second.children.clear();

        i->second.last = timing_sample();
    }
    while(!current->openSamples.empty())
        current->openSamples.pop_back();
    current->openSamples.push_back(&current->allSamples["root"]);

    lastFrameStart = SORE_Kernel::GetGlobalMS();
}

namespace SORE_Profiler
{
    bool operator==(const sample_data* p, const sample_data& r)
    {
        return p->name == r.name;
    }
}

void SORE_Profiler::Profiler::StartSample(const Sample& sample)
{
    std::string fqn = FullyQualifiedName(sample);

    Sample_map_t::iterator it = current->allSamples.find(fqn);
    if(it == current->allSamples.end())
    {
        // create a new sample: this one hasn't been seen before
        sample_data newSample(sample.name);
        it = current->allSamples.insert(std::make_pair(fqn, newSample)).first;
    }
    if(std::find(
        current->openSamples.back()->children.begin(), 
        current->openSamples.back()->children.end(), 
        it->second) == current->openSamples.back()->children.end())
    {
        current->openSamples.back()->children.push_back(&it->second);
    }

    if(current->openSamples.back()->name != it->second.name)
        current->openSamples.push_back(&it->second);
}

void SORE_Profiler::Profiler::FinishSample(const Sample& sample)
{
    Sample_map_t::iterator it = current->allSamples.find(FullyQualifiedName());
    if(it == current->allSamples.end() || it->second.name != sample.name)
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
    sample.last.lastTime = ms;
    sample.last.totalTime += ms;

    sample.last.avgTime = (sample.last.avgTime * sample.last.timesRun + ms) / (sample.last.timesRun + 1);
    sample.last.maxTime = std::max(sample.last.maxTime, ms);
    sample.last.minTime = std::min(sample.last.minTime, ms);

    ++sample.last.timesRun;
    sample.current = true;

    current->openSamples.pop_back();
}

std::string SORE_Profiler::Profiler::FullyQualifiedName(const Sample& sample)
{
    return FullyQualifiedName() + "." + sample.name;
}

std::string SORE_Profiler::Profiler::FullyQualifiedName()
{
    std::string name;

    for(std::deque<sample_data*>::iterator i = current->openSamples.begin(); i != current->openSamples.end(); ++i)
    {
        name += ".";
        name += (*i)->name;
    }

    return name;
}