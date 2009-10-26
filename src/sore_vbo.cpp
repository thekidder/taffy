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

#include "sore_vbo.h"
#include "sore_logger.h"
#include <algorithm>

namespace SORE_Graphics
{
    VBO::VBO(bool t, bool c, bool n) :
        GraphicsArray(t, c, n), vbo(0), vboIndices(0)
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
        //glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vboIndices);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
    }

    void* VBO::GetOffset(void* pointer, unsigned int offset)
    {
        return ((char*)NULL + (offset));
    }

    void VBO::Build()
    {
        if(!indices.size()) return;

        unsigned int usage = GL_STREAM_DRAW_ARB;
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertices.size()*sizeof(vertex), &(vertices[0].x),
                        usage);

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vboIndices);
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indices.size()*sizeof(unsigned short),
                        &(indices[0]), usage);

    }
}
