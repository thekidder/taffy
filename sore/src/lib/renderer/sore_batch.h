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

#ifndef SORE_BATCH_H
#define SORE_BATCH_H

#include <sore_graphicsarray.h>
#include <sore_renderable.h>
#include <sore_texture2d.h>
#include <sore_renderstate.h>
#include <sore_screeninfo.h>
#include <sore_glslshader.h>

#include <map>

namespace SORE_Graphics
{
    struct geometry_entry
    {
        geometry_entry() : geometry(0), offset(0), indices(0), vertices(0), type(0) {}
        geometry_entry(
            GraphicsArray* geometry,
            unsigned int offset,
            unsigned int indices,
            unsigned int vertices,
            GLenum type)
            : geometry(geometry), offset(offset), indices(indices), vertices(vertices), type(type)
        {}
        GraphicsArray* geometry;
        unsigned int offset;
        unsigned int indices;
        unsigned int vertices;
        GLenum type;
    };

    class RenderBatch
    {
    public:
        // creates a no-op batch
        RenderBatch();
        // create a batch that does nothing but set the state
        RenderBatch(const RenderState& state);
        // sets command to either NONE or BIND_VBO
        RenderBatch(const geometry_entry& geometry,
                    const RenderState& state, bool bindVBO = false);

        void AddIndices(const geometry_entry& geometry);
        // returns number of polygons rendered
        unsigned int Render();

        void EndDraw() const; // call at end of frame
    private:
        GraphicsArray* geometry;
        GLenum type;
        bool bind;

        // (start offset, num indices)
        typedef std::map<unsigned int, unsigned int> Geometry_ranges_t;
        Geometry_ranges_t ranges;

        RenderState state;
    };
}

#endif
