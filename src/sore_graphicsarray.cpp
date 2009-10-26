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

#include "sore_graphicsarray.h"
#include "sore_logger.h"

namespace SORE_Graphics
{
    GraphicsArray::GraphicsArray(bool t, bool c, bool n) :
        hasTexCoords(t), hasNormals(n), hasColors(c)
    {
    }

    void GraphicsArray::Clear()
    {
        vertices.clear();
        indices.clear();
    }

    void GraphicsArray::AddObject(const GLfloat* v, const unsigned short* ind,
                                  unsigned int numVertices, unsigned int numIndices,
                                  const SORE_Math::Matrix4<float>* transform,
                                  const GLfloat* t, const GLfloat* n, const GLfloat* c)
    {
        if( (hasNormals && !n) || (hasTexCoords && !t) || (hasColors && !c) )
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Invalid VBO format; check constructer");
        }
        size_t oldSize = vertices.size();

        vertices.resize(vertices.size() + numVertices);
        for(unsigned int i=0;i<numVertices;++i)
        {
            SORE_Math::Vector4<float> pos(v[i*3], v[i*3+1],v[i*3+2], 1.0f);
            pos = *transform * pos;

            vertices[oldSize + i].x = pos[0];
            vertices[oldSize + i].y = pos[1];
            vertices[oldSize + i].z = pos[2];

            if(hasColors)
            {
                vertices[oldSize + i].r = c[i*4];
                vertices[oldSize + i].g = c[i*4+1];
                vertices[oldSize + i].b = c[i*4+2];
                vertices[oldSize + i].a = c[i*4+3];
            }
            if(hasTexCoords)
            {
                vertices[oldSize + i].texi = t[i*2+0];
                vertices[oldSize + i].texj = t[i*2+1];
            }
            if(hasNormals)
            {
                vertices[oldSize + i].normx = n[i*3];
                vertices[oldSize + i].normy = n[i*3+1];
                vertices[oldSize + i].normz = n[i*3+2];
            }
        }

        //copy vertices into VBO, taking into account index renumbering
        for(unsigned short j=0;j<numIndices;++j)
        {
            unsigned short index = ind[j] + oldSize;
            indices.push_back(index);
        }
    }

    void GraphicsArray::BeginDraw()
    {
        BeginDrawHook();

        glEnableClientState(GL_VERTEX_ARRAY);
        if(hasColors)
          glEnableClientState(GL_COLOR_ARRAY);
        if(hasTexCoords)
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        if(hasNormals)
            glEnableClientState(GL_NORMAL_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(vertex), GetOffset(&(vertices[0].x), 0));
        if(hasTexCoords)
        {
            glClientActiveTexture(GL_TEXTURE0);
            glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), GetOffset(&(vertices[0].x), 12));
        }
        if(hasNormals)
        {
            glNormalPointer(GL_FLOAT, sizeof(vertex), GetOffset(&(vertices[0].x), 20));
        }
        if(hasColors)
        {
            glColorPointer(4, GL_FLOAT, sizeof(vertex), GetOffset(&(vertices[0].x), 32));
        }
    }

    void GraphicsArray::EndDraw()
    {
        if(hasColors)
            glDisableClientState(GL_COLOR_ARRAY);
        if(hasNormals)
            glDisableClientState(GL_NORMAL_ARRAY);
        if(hasTexCoords)
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    void GraphicsArray::DrawAll()
    {
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT,
                       GetOffset(&(indices[0]), 0));
    }

    void GraphicsArray::DrawElements(unsigned int numTris, unsigned short triOffset)
    {
        /*glDrawElements(GL_TRIANGLES, numTris*3, GL_UNSIGNED_SHORT,
          GetOffset(&(indices[0]), triOffset*3));*/
        glDrawRangeElements(GL_TRIANGLES, triOffset*3, indices.size(), numTris*3, GL_UNSIGNED_SHORT,
                            GetOffset(&(indices[0]), triOffset*3));
    }

    bool GraphicsArray::Empty() const
    {
        return indices.empty();
    }
}
