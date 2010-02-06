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

#include <algorithm>

#include "sore_allgl.h"
#include "sore_renderer_std.h"
#include "sore_shaders.h"

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

    MakeBatches(allRenderables, batches, geometry, false);
}

void SORE_Graphics::Renderer::BuildStatic()
{
    staticBatches.clear();
    ClearGeometry(staticGeometry);
    staticGeometry.clear();
    MakeBatches(staticRenderables, staticBatches, staticGeometry, true);
}

void SORE_Graphics::Renderer::MakeBatches(
    std::vector<Renderable>& allRenderables, 
    std::vector<RenderBatch>& batches,
    std::vector<GraphicsArray*>& geometry,
    bool isStatic)
{
        std::sort(allRenderables.begin(), allRenderables.end());

    //loop through all renderables, building VBOs and draw call commands
    std::vector<Renderable>::iterator r_it;
    unsigned int vboSize = 0, numIndices = 0, offset = 0;
    if(geometry.empty())
    {
        GraphicsArray* ga = new GraphicsArrayClass(isStatic, true, true);
        geometry.push_back(ga);
    }
    Renderable old = allRenderables.front();
    std::vector<GraphicsArray*>::iterator thisGeometry = geometry.begin();
    for(r_it = allRenderables.begin(); r_it != allRenderables.end(); ++r_it)
    {
        if(r_it->GetGeometryChunk()->NumIndices() + vboSize > 65535)
        {
            (*thisGeometry)->Build();
            batches.back().SetNumIndices(numIndices);
            batches.back().SetIndexOffset(offset);
            thisGeometry++;
            vboSize = numIndices = offset = 0;
            if(thisGeometry == geometry.end())
            {
                GraphicsArray* ga = new GraphicsArrayClass(isStatic, true, true);
                geometry.push_back(ga);
                thisGeometry = geometry.end() - 1;
            }
        }
        (*thisGeometry)->AddObject(r_it->GetGeometryChunk(), r_it->GetTransform());

        UniformState u;
        if(r_it != allRenderables.begin())
            u = r_it->Uniforms().GetDiff(old.Uniforms());
        else
            u = r_it->Uniforms();

        bool bindVBO = false, bindShader = false, bindTexture = false;
        bool changeBlend = false, changeCamera = false, changeUniforms = false;
        bool changeType = false;
        if(vboSize == 0)
            bindVBO = true;
        if(r_it == allRenderables.begin() || *r_it->GetShader() != *old.GetShader())
            bindShader = true;
        TextureState t;
        if(r_it != allRenderables.begin())
            t = r_it->GetTextures().GetDiff(old.GetTextures());
        else
            t = r_it->GetTextures();
        if(r_it->GetGeometryChunk()->Type() != old.GetGeometryChunk()->Type())
            changeType = true;
        if(!t.Empty())
            bindTexture = true;
        if(r_it == allRenderables.begin() ||
           r_it->GetBlendMode() != old.GetBlendMode())
            changeBlend = true;
        if(!u.Empty())
            changeUniforms = true;
        if(r_it == allRenderables.begin() || r_it->GetLayer() != old.GetLayer())
            changeCamera = true;
        if(bindVBO || bindShader || bindTexture || changeBlend
           || changeCamera || changeUniforms || changeType)
        {
            if(!batches.empty() && !bindVBO)
            {
                batches.back().SetNumIndices(numIndices);
                batches.back().SetIndexOffset(offset);
            }
            batches.push_back(RenderBatch(*thisGeometry,
                                          bindVBO));
            batches.back().SetLayer(r_it->GetLayer());
            offset += numIndices;

            if(changeCamera)
                batches.back().AddChangeCameraCommand(
                    GetCamera(r_it->GetLayer()));
            if(changeBlend)
                batches.back().AddChangeBlendModeCommand(r_it->GetBlendMode());
            if(bindShader)
                batches.back().AddBindShaderCommand(r_it->GetShader());
            if(bindTexture)
                batches.back().AddBindTextureCommand(
                    r_it->GetShader(), t);
            if(changeUniforms)
                batches.back().AddChangeUniformsCommand(r_it->GetShader(), u);
            batches.back().SetType(r_it->GetGeometryChunk()->Type());
            numIndices = 0;
        }
        vboSize += r_it->GetGeometryChunk()->NumIndices();
        numIndices += r_it->GetGeometryChunk()->NumIndices();
        old = *r_it;
    }
    (*thisGeometry)->Build();
    batches.back().SetNumIndices(numIndices);
    batches.back().SetIndexOffset(offset);
}

void SORE_Graphics::Renderer::Render()
{
    Build();

    numPolys = 0;
    drawCalls = 0;

    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    std::vector<RenderBatch>::iterator it, prev;
    for(it = prev = staticBatches.begin(); it != staticBatches.end(); ++it)
    {
        if((it != prev && it->GetLayer() != prev->GetLayer()) || it == prev)
        {
            it->AddChangeCameraCommand(GetCamera(it->GetLayer()));
        }
        numPolys += it->Render();
        drawCalls++;

        prev = it;
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
