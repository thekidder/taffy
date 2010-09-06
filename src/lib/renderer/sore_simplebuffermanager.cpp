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

#include <cassert>
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
    for(unsigned int i = 0; i < MAX_GEOMETRY_TYPE; ++i)
    {
        BOOST_FOREACH(geometry_buffer* buf, heaps[i])
        {
            delete buf;
        }
    }
}

/*SORE_Graphics::render_queue SORE_Graphics::SimpleBufferManager::GetRenderList()
{
    render_queue rq = {allRenderables, rMap};
    rq.renderables = allRenderables;
    rq.buffers = rMap;

    return rq;
}*/

SORE_Graphics::geometry_entry SORE_Graphics::SimpleBufferManager::LookupGC(GeometryChunkPtr gc)
{
    if(geometryMapping.find(gc) == geometryMapping.end())
    {
        ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not find geometry");
        geometry_entry ge;
        return ge;
    }
    geometry_buffer* gb = geometryMapping[gc];
    if(gb->geometryMap.find(gc) == gb->geometryMap.end())
    {
        ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not lookup GC in buffer");
        geometry_entry ge;
        return ge;
    }
    return gb->geometryMap[gc];
}

void SORE_Graphics::SimpleBufferManager::GeometryAdded(GeometryChunkPtr gc, geometry_type type)
{
    geometry_buffer* buffer = Insert(gc, type);
    
    geometry_entry e = 
    {
        &buffer->buffer,
        buffer->buffer.NumIndices() - gc->NumIndices(),
        gc->NumIndices(),
        gc->NumVertices(),
        gc->Type()
    };
    buffer->geometryMap[gc] = e;
}

void SORE_Graphics::SimpleBufferManager::GeometryChanged(GeometryChunkPtr gc)
{
    geometry_buffer* buffer = geometryMapping[gc];
    /*if(buffer->geometryMap[gc].indices != gc->NumIndices() || 
       buffer->geometryMap[gc].vertices != gc->NumVertices())
    {
        //size of the geometry has changed, reload the entire VBO
    }
    else
    {
        //do an in-place upload of the changed data
    }*/
    //too big for current heap?
    size_t newIndices = gc->NumIndices() - buffer->geometryMap[gc].indices;
    size_t newVertices = gc->NumVertices() - buffer->geometryMap[gc].vertices;
    if(!buffer->buffer.HasRoomFor(newIndices, newVertices))
    {
        assert(0);
        buffer->geometryMap.erase(gc);
        GeometryAdded(gc, DYNAMIC); //placeholder
    }
    RebuildBuffer(buffer);
}

void SORE_Graphics::SimpleBufferManager::GeometryRemoved(GeometryChunkPtr gc)
{
    geometry_buffer* buffer = geometryMapping[gc];
    buffer->geometryMap.erase(gc);
    RebuildBuffer(buffer);
}

void SORE_Graphics::SimpleBufferManager::RebuildBuffer(geometry_buffer* buffer)
{
    buffer->buffer.Clear();
    boost::unordered_map<GeometryChunkPtr, geometry_entry>::iterator it;
    for(it = buffer->geometryMap.begin(); it != buffer->geometryMap.end(); ++it)
    {
        buffer->buffer.AddObject(it->first);
        buffer->geometryMap[it->first].offset = buffer->buffer.NumIndices() - it->first->NumIndices();
    }
}

SORE_Graphics::SimpleBufferManager::geometry_buffer* SORE_Graphics::SimpleBufferManager::Insert(GeometryChunkPtr g, geometry_type type)
{
    std::vector<geometry_buffer*>& heap = heaps[type];
    geometry_buffer* current = heap.back();
    if(!current->buffer.HasRoomFor(g->NumIndices(), g->NumVertices()))
    {
        current = new geometry_buffer(type);
        heap.push_back(current);
    }

    current->buffer.AddObject(g);

    return current;
}