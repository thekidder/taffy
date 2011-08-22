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

#include <sore_font.h>
#include <sore_profiler.h>
#include <sore_profilerstats.h>
#include <sore_textwidget.h>

#include <boost/format.hpp>

SORE_GUI::ProfilerStats::ProfilerStats(SVec s, SVec p, SORE_Profiler::Profiler& profiler_, Widget *parent)
    : FrameWindow(s, p, "Profiler Stats", parent), profiler(profiler_)
{
    content = new Widget(SVec(1.0, 1.0), SVec(0, 0), this);
}

void SORE_GUI::ProfilerStats::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
{
    RenderFrame(imm_mode);
    content->RemoveChildren();

    const SORE_Profiler::sample_data* sample = profiler.Samples();

    if(sample)
        RenderSample(imm_mode, sample);
}

struct SampleTimeComparator
{
    bool operator()(const SORE_Profiler::sample_data* one, const SORE_Profiler::sample_data* two) const
    {
        return one->total.lastTime > two->total.lastTime;
    }
};

int SORE_GUI::ProfilerStats::RenderSample(
    SORE_Graphics::ImmediateModeProvider& imm_mode,
    const SORE_Profiler::sample_data* sample, double totalTime, int treeLevel, int height)
{
    if(totalTime == 0.0f)
        totalTime = sample->total.lastTime;

    const unsigned int TEXT_WIDTH = 200;
    const unsigned int TIME_WIDTH = 60;
    unsigned int width = content->GetSize(SORE_GUI::HORIZONTAL) - (TEXT_WIDTH + TIME_WIDTH);

    float bar_width = static_cast<float>(sample->total.lastTime / totalTime);
    bar_width = std::min(bar_width, 1.0f);

    new TextWidget(16, SVec(10 + treeLevel * 4, height), content, sample->name);
    float x1 = static_cast<float>(TEXT_WIDTH + TIME_WIDTH);
    float x2 = x1 + bar_width * static_cast<float>(width);
    float y1 = static_cast<float>(height + 1);
    float y2 = y1 + 16.0f;
    imm_mode.SetBlendMode(SORE_Graphics::BLEND_SUBTRACTIVE);
    imm_mode.SetShader(shaderCache.Get("untextured.shad"));
    imm_mode.SetTexture(SORE_Resource::Texture2DPtr());
    imm_mode.SetColor(SORE_Graphics::White);
    imm_mode.SetTransform(SORE_Graphics::TransformationPtr(new SORE_Math::Matrix4<float>(content->GetPositionMatrix())));
    imm_mode.DrawQuad(
        x1, y1, 0.0f,
        x1, y2, 0.0f,
        x2, y1, 0.0f,
        x2, y2, 0.0f);

    std::string ms = (boost::format("%.1f ms") % sample->total.lastTime).str();
    TextWidget* t = new TextWidget(16, SVec((int)TEXT_WIDTH, height), content, ms);

    height += 18;

    std::vector<SORE_Profiler::sample_data*> children = sample->children;
    std::sort(children.begin(), children.end(), SampleTimeComparator());

    for(std::vector<SORE_Profiler::sample_data*>::const_iterator i = children.begin(); i != children.end(); ++i)
    {
        height = RenderSample(imm_mode, *i, totalTime, treeLevel + 1, height);
    }
    return height;
}