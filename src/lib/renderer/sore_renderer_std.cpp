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

#include <algorithm>

#include <sore_allgl.h>
#include <sore_renderer_std.h>
#include <sore_glslshader.h>

inline int next_p2 (int a )
{
    int rval=1;
    while(rval<a) rval<<=1;
    return rval;
}

SORE_Graphics::Renderer::Renderer(SORE_Resource::ResourcePool& _pool)
    : pool(_pool)
{
    glEnable(GL_BLEND);
    PushState();
}

SORE_Graphics::Renderer::~Renderer()
{
    std::vector<GraphicsArray*>::iterator it;
    for(it = geometry.begin(); it != geometry.end(); ++it)
    {
        delete *it;
    }
    for(it = staticGeometry.begin(); it != staticGeometry.end(); ++it)
    {
        delete *it;
    }
}

void SORE_Graphics::Renderer::OnScreenChange()
{
}

void SORE_Graphics::Renderer::ClearGeometry(std::vector<GraphicsArray*>& geometry)
{
    std::vector<GraphicsArray*>::iterator it;
    for(it = geometry.begin(); it != geometry.end(); ++it)
    {
        (*it)->Clear();
    }
}

void SORE_Graphics::Renderer::AddStaticGeometry(SORE_Graphics::Renderable r)
{
    staticRenderables.push_back(r);
    BuildStatic();
}

#ifndef SORE_NO_VBO
#define GraphicsArrayClass VBO
#else
#define GraphicsArrayClass VertexArray
#endif

void SORE_Graphics::Renderer::Build()
{
    ClearGeometry(geometry);
    batches.clear();
    if(!currentState->providers.size())
        return;

    std::vector<Renderable> allRenderables;

    std::vector<GeometryProvider*>::iterator it;
    for(it = currentState->providers.begin(); 
        it != currentState->providers.end(); ++it)
    {
        (*it)->MakeUpToDate();
        std::copy((*it)->GeometryBegin(), (*it)->GeometryEnd(),
                  std::back_inserter(allRenderables));
    }

    if(allRenderables.empty())
        return;
    std::vector<Renderable>::iterator rit;
    for(rit = allRenderables.begin(); rit != allRenderables.end(); ++rit)
    {
        rit->SetProjection(SetupProjection(GetCamera(rit->GetLayer()).projection, screen));
    }

    std::sort(allRenderables.begin(), allRenderables.end());
    boost::unordered_map<Renderable, geometry_entry> geometryMap;
    BuildGeometryBuffers(allRenderables, geometry, false, geometryMap);
    MakeBatches(allRenderables, batches, geometryMap, false);

    staticBatches.clear();
    MakeBatches(staticRenderables, staticBatches, staticMap, true);
}

void SORE_Graphics::Renderer::BuildStatic()
{
    staticBatches.clear();
    ClearGeometry(staticGeometry);
    staticGeometry.clear();

    std::sort(staticRenderables.begin(), staticRenderables.end());
    BuildGeometryBuffers(staticRenderables, staticGeometry, true, staticMap);
}

void SORE_Graphics::Renderer::MakeBatches(
    std::vector<Renderable>& allRenderables, 
    std::vector<RenderBatch>& batches,
    boost::unordered_map<Renderable, geometry_entry>& geometryMap,
    bool isStatic)
{
    //loop through all renderables, building VBOs and draw call commands
    unsigned int numIndices = 0, currentOffset = 0;
    std::vector<Renderable>::iterator it, prev = allRenderables.begin();
    RenderState oldState;
    geometry_entry previous;
    previous.geometry = NULL;
    for(it = allRenderables.begin(); it != allRenderables.end(); ++it)
    {
        geometry_entry current = geometryMap[*it];
        RenderState newState = oldState.Difference(*it, GetCamera(it->GetLayer()));

        if(previous.geometry != current.geometry)
        {
            batches.push_back(RenderBatch(current, newState, true));
        }
        else if(current.offset != (previous.offset + previous.num))
        {
            batches.push_back(RenderBatch(current, newState, false));
        }
        else if(!newState.Empty())
        {
            batches.push_back(RenderBatch(current, newState, false));
        }
        else
        {
            batches.back().AddIndices(current.num);
        }

        previous = current;
        oldState = newState;
    }
}

namespace boost
{
    static std::size_t hash_value(const SORE_Graphics::Renderable& r)
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, r.GetGeometryChunk());

        return seed;
    }
}

namespace SORE_Graphics
{
    static bool operator==(const SORE_Graphics::Renderable& one,
                           const SORE_Graphics::Renderable& two)
    {
        return one.GetGeometryChunk() == two.GetGeometryChunk()
            && *one.GetTransform() == *two.GetTransform();
    }
}

void SORE_Graphics::Renderer::BuildGeometryBuffers(
    std::vector<Renderable>& allRenderables,
    std::vector<GraphicsArray*>& geometry,
    bool isStatic,
    boost::unordered_map<Renderable, geometry_entry>& result)
{
    unsigned int vboSize = 0;
    if(geometry.empty())
    {
        GraphicsArray* ga = new GraphicsArrayClass(isStatic, true, true);
        geometry.push_back(ga);
    }

    std::vector<GraphicsArray*>::iterator thisGeometry = geometry.begin();
    std::vector<Renderable>::iterator it;
    for(it = allRenderables.begin(); it != allRenderables.end(); ++it)
    {
        if(it->GetGeometryChunk()->NumIndices() + vboSize > 65535)
        {
            (*thisGeometry)->Build();
            thisGeometry++;
            vboSize = 0;
            if(thisGeometry == geometry.end())
            {
                GraphicsArray* ga = new GraphicsArrayClass(isStatic, true, true);
                geometry.push_back(ga);
                thisGeometry = geometry.end() - 1;
            }
        }
        (*thisGeometry)->AddObject(it->GetGeometryChunk(), it->GetTransform());
        geometry_entry e = {
            *thisGeometry, 
            vboSize, 
            it->GetGeometryChunk()->NumIndices(),
            it->GetGeometryChunk()->Type()};
        const Renderable& r = *it;
        result[r] = e;
        vboSize += it->GetGeometryChunk()->NumIndices();
    }
    geometry.back()->Build();
}

void SORE_Graphics::Renderer::Render()
{
    Build();

    numPolys = 0;
    drawCalls = 0;

    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    std::vector<RenderBatch>::iterator it;
    for(it = staticBatches.begin(); it != staticBatches.end(); ++it)
    {
        numPolys += it->Render();
        drawCalls++;
    }
    for(it = batches.begin(); it != batches.end(); ++it)
    {
        numPolys += it->Render();
        drawCalls++;
    }
    if(drawCalls > 0)
        geometry.back()->EndDraw();
    CalculateFPS();

    PrintGLErrors(SORE_Logging::LVL_ERROR);
}

void SORE_Graphics::Renderer::SetCamera(
    geometry_layer layer, camera_callback camera)
{
    currentState->cameras[layer] = camera;
}

SORE_Graphics::camera_info SORE_Graphics::Renderer::GetCamera(geometry_layer layer)
{
    std::map<geometry_layer, camera_callback>::iterator it;
    it = currentState->cameras.find(layer);
    if(it != currentState->cameras.end())
    {
        camera_info temp = it->second();
        temp.projection = SetupProjection(temp.projection, screen);
        return temp;
    }
    camera_info none;
    return none;
}


void SORE_Graphics::Renderer::CalculateFPS()
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

float SORE_Graphics::Renderer::GetFPS() const
{
    return fps;
}

float SORE_Graphics::Renderer::GetFrameMS() const
{
    return ms;
}

unsigned int SORE_Graphics::Renderer::GetDrawCalls() const
{
    return drawCalls;
}

unsigned int SORE_Graphics::Renderer::GetNumPolys() const
{
    return numPolys;
}

void SORE_Graphics::Renderer::ClearGeometryProviders()
{
    currentState->providers.clear();
}

void SORE_Graphics::Renderer::AddGeometryProvider(GeometryProvider* gp)
{
    currentState->providers.push_back(gp);
}

void SORE_Graphics::Renderer::PushState()
{
    render_info temp;
    geometryProviders.push_back(temp);
    currentState = geometryProviders.end() - 1;
}

void SORE_Graphics::Renderer::PopState()
{
    geometryProviders.pop_back();
    currentState = geometryProviders.end() - 1;
}
