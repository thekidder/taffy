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

#ifndef SORE_SIMPLEBUFFERMANAGER_H
#define SORE_SIMPLEBUFFERMANAGER_H

#include <boost/unordered_map.hpp>

#include <sore_buffermanager.h>
#include <sore_util.h>

#ifndef SORE_NO_VBO
#include <sore_vbo.h>
#define GraphicsArrayClass VBO
#else
#include <sore_vertexarray.h>
#define GraphicsArrayClass VertexArray
#endif

namespace SORE_Graphics
{
    class SORE_EXPORT SimpleBufferManager : public BufferManager, SORE_Utility::Noncopyable
    {
    public:
        SimpleBufferManager();
        ~SimpleBufferManager();

        //renderer interface
        virtual void MakeUpToDate();
        virtual geometry_entry LookupGC(GeometryChunkPtr gc);
        virtual bool Contains(GeometryChunkPtr gc);

        //game interface
        void Clear();
        void GeometryAdded(GeometryChunkPtr gc, geometry_type type);
        void GeometryChanged(GeometryChunkPtr gc);
        void GeometryRemoved(GeometryChunkPtr gc);

    private:
         struct geometry_buffer
         {
             geometry_buffer(geometry_type type) : buffer(type, true, true, false) {}

             GraphicsArrayClass buffer;
             bool needsRebuild;
             //generated from renderables
             typedef boost::unordered_map<GeometryChunkPtr, geometry_entry> Geometry_map_t;
             Geometry_map_t geometryMap;
        };

        geometry_buffer* Insert(GeometryChunkPtr g, geometry_type type);
        void RebuildBuffer(geometry_buffer* buffer);

        std::vector<geometry_buffer*> heaps[MAX_GEOMETRY_TYPE];
        //indexes a GC into one of the buffers above ^
        boost::unordered_map<GeometryChunkPtr, geometry_buffer*> geometryMapping;

        renderable_map renderables;
    };
}

#endif
