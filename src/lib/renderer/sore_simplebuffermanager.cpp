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

#include <limits>

#include <boost/foreach.hpp>

#include <sore_simplebuffermanager.h>
#include <sore_vbo.h>

#ifndef SORE_NO_VBO
#define GraphicsArrayClass VBO
#else
#define GraphicsArrayClass VertexArray
#endif

SORE_Graphics::SimpleBufferManager::SimpleBufferManager()
{
}

SORE_Graphics::SimpleBufferManager::~SimpleBufferManager()
{
    for(unsigned int i = 0; i < 3; ++i)
    {
        std::vector<geometry_buffer>& b = buffers[i];
        BOOST_FOREACH(geometry_buffer ga, b)
        {
            delete ga.buffer;
        }
    }
}

SORE_Graphics::render_queue SORE_Graphics::SimpleBufferManager::GetRenderList()
{
    render_queue rq = {allRenderables, rMap};
    rq.renderables = allRenderables;
    rq.buffers = rMap;

    return rq;
}

void SORE_Graphics::SimpleBufferManager::GeometryAdded(GeometryChunkPtr gc, geometry_type type)
{
    allRenderables.push_back(r);

    if(rMap.find(r.GetGeometryChunk()) == rMap.end())
    {
        Insert(r.GetGeometryChunk(), type);
    }
}

void SORE_Graphics::SimpleBufferManager::GeometryChanged(GeometryChunkPtr gc)
{
}

void SORE_Graphics::SimpleBufferManager::GeometryRemoved(GeometryChunkPtr gc)
{
}

void SORE_Graphics::SimpleBufferManager::Insert(GeometryChunkPtr g, geometry_type type)
{
    std::vector<geometry_buffer>& buffer = buffers[type];
    GraphicsArray* current = buffer.back().buffer;
    if(current->NumIndices() + g->NumIndices() > std::numeric_limits<unsigned short>::max() ||
        current->NumVertices() + g->NumVertices() > std::numeric_limits<unsigned short>::max())
    {
        current = new GraphicsArrayClass(type, true, true, false);
        buffer.push_back(current);
    }

    current->AddObject(g);
    /*geometry_entry e = 
    {
        *thisGeometry,
        vboSize,
        it->GetGeometryChunk()->NumIndices(),
        it->GetGeometryChunk()->Type()
    };*/
}