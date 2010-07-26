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

#include <sore_vbo.h>
#include <sore_logger.h>
#include <algorithm>

namespace SORE_Graphics
{
    VBO::VBO(geometry_type type, bool t, bool c, bool n) :
        GraphicsArray(type, t, c, n), vbo(0), vboIndices(0)
    {
        glGenBuffersARB(1, &vbo);
        if(!vbo)
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create vertex buffer");
        glGenBuffersARB(1, &vboIndices);
        if(!vboIndices)
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create index buffer");
    }

    VBO::~VBO()
    {
        glDeleteBuffersARB(1, &vbo);
        glDeleteBuffersARB(1, &vboIndices);
    }

    void VBO::BeginDrawHook()
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vboIndices);
    }

    void* VBO::GetOffset(void* pointer, unsigned int offset)
    {
        return ((char*)NULL + (offset));
    }

    void VBO::Build()
    {
        if(!indices.size()) return;

        unsigned int usage;
        switch(type)
        {
        case STATIC:
            usage = GL_STATIC_DRAW_ARB;
            break;
        case DYNAMIC:
            usage = GL_DYNAMIC_DRAW_ARB;
            break;
        case STREAM:
            usage = GL_STREAM_DRAW_ARB;
            break;
        }

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                        vertices.size()*sizeof(vertex),
                        &(vertices[0]), usage);

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vboIndices);
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                        indices.size()*sizeof(unsigned short),
                        &(indices[0]), usage);

    }
}
