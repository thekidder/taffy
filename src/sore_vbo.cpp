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
        GraphicsArray(t, c, n), vbo(0), vboIndices(0), vboTexCoords(0), vboColors(0), vboNormals(0)
    {
        glGenBuffersARB(1, &vbo);
        if(!vbo)
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create vertex buffer");
        glGenBuffersARB(1, &vboIndices);
        if(!vboIndices)
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create index buffer");
        if(t)
        {
            glGenBuffersARB(1, &vboTexCoords);
            if(!vboTexCoords)
                ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create texture coordinate buffer");
        }
        if(c)
        {
            glGenBuffersARB(1, &vboColors);
            if(!vboColors)
                ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create color buffer");
        }
        if(n)
        {
            glGenBuffersARB(1, &vboNormals);
            if(!vboNormals)
                ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create normal buffer");
        }
    }

    VBO::~VBO()
    {
        glDeleteBuffersARB(1, &vbo);
        glDeleteBuffersARB(1, &vboIndices);
        if(hasColors)
            glDeleteBuffersARB(1, &vboColors);
        if(hasNormals)
            glDeleteBuffersARB(1, &vboNormals);
        if(hasTexCoords)
        glDeleteBuffersARB(1, &vboTexCoords);
    }

    void VBO::BeginDrawHook()
    {
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vboIndices);

        if(hasNormals)
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboNormals);
        }
        if(hasTexCoords)
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboTexCoords);
        }
        if(hasColors)
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboColors);
        }
    }

    void* VBO::GetOffset(void* pointer, unsigned int offset)
    {
        return (char*)NULL + offset*sizeof(unsigned short);
    }

    void VBO::Build()
    {
        unsigned int usage = GL_STREAM_DRAW_ARB;
        if(!indices.size()) return;
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vboIndices);
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indices.size()*sizeof(unsigned short),
                        &(indices[0]), GL_STREAM_DRAW_ARB);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertices.size()*sizeof(GLfloat), &(vertices[0]),
                        usage);

        if(hasColors)
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboColors);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, colors.size()*sizeof(GLfloat), &(colors[0]),
                            usage);
        }

        if(hasTexCoords)
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboTexCoords);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, texCoords.size()*sizeof(GLfloat), &(texCoords[0]),
                            usage);
        }
        if(hasNormals)
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboNormals);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, normals.size()*sizeof(GLfloat), &(normals[0]),
                            usage);
        }
    }
}
