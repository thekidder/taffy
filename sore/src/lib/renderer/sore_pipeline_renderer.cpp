/**************************************************************************
 * Copyright 2011 Adam Kidder. All rights reserved.                       *
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

// uncomment to enable correct profiling results of opengl render calls
// does result in performance hit
#define OPENGL_PROFILE

#include <sore_aggregatebuffermanager.h>
#include <sore_fbo.h>
#include <sore_gl_command_list.h>
#include <sore_glslshader.h>
#include <sore_material.h>
#include <sore_pipeline_renderer.h>
#include <sore_texture2d_fboloader.h>
#include <sore_timing.h>

#include <boost/foreach.hpp>

SORE_Graphics::PipelineRenderer::PipelineRenderer(SORE_Profiler::Profiler* profiler_)
    : profiler(profiler_)
{
    boost::shared_ptr<Pipe> root(new NullPipe());
    pipeline = root;

    // default GL state
    glEnable(GL_BLEND);
    glClearColor(0.0,0.0,0.0,1.0);
}

SORE_Graphics::PipelineRenderer::~PipelineRenderer()
{
}

void SORE_Graphics::PipelineRenderer::Render()
{
    PROFILE_BLOCK("Renderer", profiler);

    AggregateBufferManager bufferManager;
    render_list renderables;

    {
        PROFILE_BLOCK("Collect Geometry", profiler);
        BOOST_FOREACH(GeometryProvider* gp, geometry)
        {
            gp->MakeUpToDate();
            bufferManager.AddBufferManager(gp->GetBufferManager());
            std::copy(gp->GeometryBegin(), gp->GeometryEnd(), std::back_inserter(renderables));
        }
    }

    ScreenInfo screenInfo;
    screenInfo.width = width;
    screenInfo.height = height;
    screenInfo.ratio = static_cast<float>(width) / height;

    // get current cameras
    camera_table cameraTable;
    typedef std::pair<std::string, camera_callback> cam;
    BOOST_FOREACH(cam c, cameras)
    {
        cameraTable[c.first] = c.second();
        cameraTable[c.first].projection = SetupProjection(cameraTable[c.first].projection, screenInfo);
    }

    // setup render buffers map
    Renderbuffer_map_t renderbuffers;

    GLCommandList renderQueue(width, height);
    renderQueue.AddCommand(CLEAR_COLOR_AND_DEPTH_BUFFERS);
    {
        PROFILE_BLOCK("Setup pipeline", profiler);
        pipeline->Setup(renderbuffers);
    }

    // ensure textures are all ready (bind references to renderbuffers)
    SORE_Resource::Texture2DFBOLoader loader(renderbuffers);

    BOOST_FOREACH(Renderable& r, renderables)
    {
        typedef std::pair<std::string, SORE_Resource::GLSLShader::glsl_variable_info> variable_info;
        BOOST_FOREACH(const variable_info& var, r.GetMaterial()->GetShader()->ActiveUniforms())
        {
            if(!var.second.bound)
            {
                SORE_Resource::Texture2D* tex = loader.Load(var.first);
                if(tex)
                    r.SetTexture(var.first, tex);
            }
        }
    }

    {
        PROFILE_BLOCK("Render pipeline", profiler);
        pipeline->Render(cameraTable, renderbuffers, renderables, renderQueue, &bufferManager);
    }

    {
        PROFILE_BLOCK("OpenGL render", profiler);
        renderQueue.Render();
#ifdef OPENGL_PROFILE
        glFinish();
#endif
        FBO::Unbind();
    }

    // collect rendering stats
    numPolys = renderQueue.NumPolygons();
    numDrawCalls = renderQueue.NumDrawCalls();

    CalculateFPS();
}

void SORE_Graphics::PipelineRenderer::ClearGeometryProviders()
{
    geometry.clear();
}

void SORE_Graphics::PipelineRenderer::AddGeometryProvider(GeometryProvider* gp)
{
    geometry.push_back(gp);
}

void SORE_Graphics::PipelineRenderer::SetCameraTable(const camera_callback_table& cameras_)
{
    cameras = cameras_;
}

bool SORE_Graphics::PipelineRenderer::OnResize(const SORE_Kernel::Event& e)
{
    if(e.type == SORE_Kernel::RESIZE)
    {
        width = e.resize.w;
        height = e.resize.h;
        return true;
    }
    return false;
}

boost::shared_ptr<SORE_Graphics::Pipe>& SORE_Graphics::PipelineRenderer::RootPipe()
{
    return pipeline;
}

void SORE_Graphics::PipelineRenderer::CalculateFPS()
{
    static int frames = 0;
    static int T0 = SORE_Kernel::GetGlobalTicks();
    static int last;
    frames++;

    GLint t = SORE_Kernel::GetGlobalTicks();

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
