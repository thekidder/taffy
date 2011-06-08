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

#include <sore_exception.h>
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

bool SORE_Graphics::SimpleBufferManager::Contains(GeometryChunkPtr gc)
{
    return !(geometryMapping.find(gc) == geometryMapping.end());
}

SORE_Graphics::geometry_entry SORE_Graphics::SimpleBufferManager::LookupGC(GeometryChunkPtr gc)
{
    if(!Contains(gc))
    {
        throw SORE_Error::Error("Could not find geometry");
    }
    geometry_buffer* gb = geometryMapping[gc];
    if(gb->geometryMap.find(gc) == gb->geometryMap.end())
    {
        throw SORE_Error::Error("Could not lookup GC in buffer");
    }
    return gb->geometryMap[gc];
}

void SORE_Graphics::SimpleBufferManager::MakeUpToDate()
{
    for(unsigned int i = 0; i < MAX_GEOMETRY_TYPE; ++i)
    {
        BOOST_FOREACH(geometry_buffer* buffer, heaps[i])
        {
            if(buffer->needsRebuild)
            {
                RebuildBuffer(buffer);
                buffer->needsRebuild = false;
            }
        }
    }
}

void SORE_Graphics::SimpleBufferManager::GeometryAdded(GeometryChunkPtr gc, geometry_type type)
{
    if(Contains(gc))
    {
        GeometryChanged(gc);
    }
    else
    {
        geometry_buffer* buffer = Insert(gc, type);

        geometry_entry e(
            &buffer->buffer,
            buffer->buffer.NumIndices() - gc->NumIndices(),
            gc->NumIndices(),
            gc->NumVertices(),
            gc->Type());
        buffer->geometryMap[gc] = e;

        buffer->needsRebuild = true;
    }
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
        ENGINE_LOG(SORE_Logging::LVL_INFO, "readding geometry: not enough room");
        buffer->geometryMap.erase(gc);
        GeometryAdded(gc, DYNAMIC); //placeholder
    }
    buffer->needsRebuild = true;
}

void SORE_Graphics::SimpleBufferManager::GeometryRemoved(GeometryChunkPtr gc)
{
    geometry_buffer* buffer = geometryMapping[gc];
    buffer->geometryMap.erase(gc);
    buffer->needsRebuild = true;
}

void SORE_Graphics::SimpleBufferManager::Regenerate()
{
#ifndef SORE_NO_VBO
    for(unsigned int i = 0; i < MAX_GEOMETRY_TYPE; ++i)
    {
        BOOST_FOREACH(geometry_buffer* buffer, heaps[i])
        {
            buffer->buffer.Regenerate();
        }
    }
#endif
}

void SORE_Graphics::SimpleBufferManager::Clear()
{
    geometryMapping.clear();
    for(unsigned int i = 0; i < MAX_GEOMETRY_TYPE; ++i)
    {
        BOOST_FOREACH(geometry_buffer* buffer, heaps[i])
        {
            delete buffer;
        }
        heaps[i].clear();
    }
}

void SORE_Graphics::SimpleBufferManager::RebuildBuffer(geometry_buffer* buffer)
{
    //ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("rebuilding geometry buffer %p") % buffer);
    buffer->buffer.Clear();
    geometry_buffer::geometry_map::iterator it;
    for(it = buffer->geometryMap.begin(); it != buffer->geometryMap.end(); ++it)
    {
        buffer->buffer.AddObject(it->first);
        buffer->geometryMap[it->first].offset = buffer->buffer.NumIndices() - it->first->NumIndices();
    }
    buffer->buffer.Build();
}

SORE_Graphics::SimpleBufferManager::geometry_buffer* SORE_Graphics::SimpleBufferManager::Insert
(
    GeometryChunkPtr g,
    geometry_type type
)
{
    std::vector<geometry_buffer*>& heap = heaps[type];
    geometry_buffer* current = heap.size() ? heap.back() : 0;
    if(heap.size() == 0 || !current->buffer.HasRoomFor(g->NumIndices(), g->NumVertices()))
    {
        current = new geometry_buffer(type);
        heap.push_back(current);
    }

    current->buffer.AddObject(g);
    geometryMapping[g] = current;

    return current;
}
