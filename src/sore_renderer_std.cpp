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
    PushState();
}

SORE_Graphics::Renderer::~Renderer()
{
    ClearGeometry();
}

void SORE_Graphics::Renderer::OnScreenChange()
{
}

void SORE_Graphics::Renderer::ClearGeometry()
{
    std::vector<GraphicsArray*>::iterator it;
    for(it = geometry.begin(); it != geometry.end(); ++it)
    {
        delete *it;
    }
    geometry.clear();
}

#ifndef SORE_NO_VBO
#define GraphicsArrayClass VBO
#else
#define GraphicsArrayClass VertexArray
#endif

void SORE_Graphics::Renderer::Build()
{
    ClearGeometry();
    batches.clear();
    if(!currentState->size())
        return;

    std::vector<Renderable> allRenderables;

    std::vector<GeometryProvider*>::iterator it;
    for(it = currentState->begin(); it != currentState->end(); ++it)
    {
        (*it)->MakeUpToDate();
        std::copy((*it)->GeometryBegin(), (*it)->GeometryEnd(),
                  std::back_inserter(allRenderables));
    }

    if(allRenderables.empty())
        return;
    std::sort(allRenderables.begin(), allRenderables.end());

    //loop through all renderables, building VBOs and draw call commands
    std::vector<Renderable>::iterator r_it;
    unsigned int vboSize = 0, numTris = 0, offset = 0, lastLen = 0;
    GraphicsArray* ga = new GraphicsArrayClass(true, true);
    geometry.push_back(ga);
    Renderable old = allRenderables.front();
    for(r_it = allRenderables.begin(); r_it != allRenderables.end(); ++r_it)
    {
        offset += lastLen;
        if(r_it->GetGeometryChunk()->NumIndices() + vboSize > 65535)
        {
            geometry.back()->Build();
            vboSize = 0;
            ga = new GraphicsArrayClass(true, true);
            geometry.push_back(ga);
        }
        geometry.back()->AddObject(r_it->GetGeometryChunk(), r_it->GetTransform());

        bool bindVBO = false, bindShader = false, bindTexture = false;
        bool changeBlend = false, changeProjection = false;
        if(vboSize == 0)
            bindVBO = true;
        if(r_it == allRenderables.begin() || *r_it->GetShader() != *old.GetShader())
            bindShader = true;
        if(r_it == allRenderables.begin() || *r_it->GetTexture() != *old.GetTexture())
            bindTexture = true;
        if(r_it == allRenderables.begin() ||
           r_it->GetBlendMode() != old.GetBlendMode())
            changeBlend = true;
        if(r_it == allRenderables.begin() || r_it->GetLayer() != old.GetLayer())
            changeProjection = true;
        if(bindVBO || bindShader || bindTexture || changeBlend || changeProjection)
        {
            if(!batches.empty())
            {
                batches.back().SetNumTriangles(numTris);
                batches.back().SetTriangleOffset(offset);
            }
            batches.push_back(RenderBatch(geometry.back(),
                                          bindVBO));

            if(changeProjection)
                batches.back().AddChangeProjectionCommand(
                    GetProjection(r_it->GetLayer()));
            if(changeBlend)
                batches.back().AddChangeBlendModeCommand(r_it->GetBlendMode());
            if(bindShader)
                batches.back().AddBindShaderCommand(r_it->GetShader());
            if(bindTexture)
                batches.back().AddBindTextureCommand(
                    r_it->GetShader(), r_it->GetTexture());
            if(vboSize == 0)
                offset = numTris = 0;
        }
        vboSize += r_it->GetGeometryChunk()->NumIndices();
        numTris += r_it->GetGeometryChunk()->NumIndices()/3;
        lastLen  = r_it->GetGeometryChunk()->NumIndices()/3;
    }
    geometry.back()->Build();
    batches.back().SetNumTriangles(numTris);
    batches.back().SetTriangleOffset(offset);
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
    for(it = batches.begin(); it != batches.end(); ++it)
    {
        it->Render(screen);
    }
    geometry.back()->EndDraw();

    CalculateFPS();

    PrintGLErrors(SORE_Logging::LVL_ERROR);
}

SORE_Graphics::ProjectionInfo SORE_Graphics::Renderer::GetProjection(
    geometry_layer layer)
{
    std::vector<GeometryProvider*>::iterator it;
    for(it = currentState->begin(); it != currentState->end(); ++it)
    {
        if((*it)->HasProjection(layer))
            return (*it)->GetProjection(layer);
    }
    ProjectionInfo none;
    none.type = NONE;
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
    currentState->clear();
}

void SORE_Graphics::Renderer::AddGeometryProvider(GeometryProvider* gp)
{
    currentState->push_back(gp);
}

void SORE_Graphics::Renderer::PushState()
{
    std::vector<GeometryProvider*> temp;
    geometryProviders.push_back(temp);
    currentState = geometryProviders.end() - 1;
}

void SORE_Graphics::Renderer::PopState()
{
    geometryProviders.pop_back();
    currentState = geometryProviders.end() - 1;
}
