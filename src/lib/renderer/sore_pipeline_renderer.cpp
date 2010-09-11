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

#include <boost/foreach.hpp>

#include <sore_pipeline_renderer.h>
#include <sore_timing.h>

SORE_Graphics::PipelineRenderer::PipelineRenderer(BufferManager* bm)
: bufferManager(bm)
{
}

SORE_Graphics::PipelineRenderer::~PipelineRenderer()
{
    delete bufferManager;
}

void SORE_Graphics::PipelineRenderer::Render()
{
    fps = ms = 0.0f;
    numPolys = numDrawCalls = 0;

    //collect geometry
    render_list renderables;
    BOOST_FOREACH(GeometryProvider* gp, states.top().geometry)
    {
        gp->MakeUpToDate();
        std::copy(gp->GeometryBegin(), gp->GeometryEnd(), std::back_inserter(renderables));
    }

    renderer_state& state = states.top();
    //get current cameras
    camera_table cameras;
    typedef std::pair<std::string, camera_callback> cam;
    BOOST_FOREACH(cam c, state.cameras)
    {
        cameras[c.first] = c.second();
    }

    //render using current pipeline
    state.pipeline->Setup();
    state.pipeline->Render(cameras, renderables);

    CalculateFPS();
}

void SORE_Graphics::PipelineRenderer::ClearGeometryProviders()
{
    states.top().geometry.clear();
}

void SORE_Graphics::PipelineRenderer::AddGeometryProvider(GeometryProvider* gp)
{
    states.top().geometry.push_back(gp);
}

void SORE_Graphics::PipelineRenderer::PushState()
{
    states.push(renderer_state());
    states.top().pipeline = boost::shared_ptr<Pipe>(new NullPipe());
    states.top().pipeline->AddChildPipe(new RenderPipe());
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
