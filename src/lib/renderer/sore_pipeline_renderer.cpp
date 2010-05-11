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
 
#include <sore_pipelineitem.h>
#include <sore_pipeline_renderer.h>

#include <sore_timing.h>

SORE_Graphics::PipelineRenderer::PipelineRenderer()
{
}

SORE_Graphics::PipelineRenderer::~PipelineRenderer()
{
}

void SORE_Graphics::PipelineRenderer::Render()
{
    fps = ms = 0.0f;
    numPolys = numDrawCalls = 0;

    CalculateFPS();
}

void SORE_Graphics::PipelineRenderer::PushState()
{
    states.push(renderer_state());
}

void SORE_Graphics::PipelineRenderer::PopState()
{
    states.pop();
}

void SORE_Graphics::PipelineRenderer::CalculateFPS()
{
    static int frames = 0;
    static int T0 = SORE_Timing::GetGlobalTicks();
    static int last;
    frames++;

    GLint t = SORE_Timing::GetGlobalTicks();

    ms = (static_cast<float>(t) - static_cast<float>(last)) / 10.0f;

    if (t - T0 >= 500) //calculate FPS every 50 milliseconds
    {
        GLfloat seconds = (GLfloat)((t - T0) / 10000.0);
        fps = frames / seconds;
        T0 = t;
        frames = 0;
    }
    last = t;
}