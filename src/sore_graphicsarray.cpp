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

SORE_Graphics::GraphicsArray::GraphicsArray(bool t, bool c, bool n) :
    hasTexCoords(t), hasNormals(n), hasColors(c)
{
}

SORE_Graphics::GraphicsArray::~GraphicsArray()
{
}

void SORE_Graphics::GraphicsArray::Clear()
{
    vertices.clear();
    indices.clear();
}

void SORE_Graphics::GraphicsArray::AddObject(
    GeometryChunkPtr geometry, boost::shared_ptr<SORE_Math::Matrix4<float> > transform)
{
    size_t oldSize = vertices.size();

    vertices.resize(vertices.size() + geometry->NumVertices());
    memcpy(&vertices[oldSize], geometry->GetVertices(),
           geometry->NumVertices()*sizeof(vertex));
    for(unsigned int i=0; i<geometry->NumVertices(); ++i)
    {
        SORE_Math::Vector4<float> pos(
            geometry->GetVertex(i).x,
            geometry->GetVertex(i).y,
            geometry->GetVertex(i).z,
            1.0f);
        pos = *transform * pos;

        vertices[oldSize + i].x = pos[0];
        vertices[oldSize + i].y = pos[1];
        vertices[oldSize + i].z = pos[2];
    }

    //copy vertices into VBO, taking into account index renumbering
    for(unsigned short j=0;j<geometry->NumIndices();++j)
    {
        unsigned short index = geometry->GetIndex(j) + oldSize;
        indices.push_back(index);
    }
}

void SORE_Graphics::GraphicsArray::BeginDraw()
{
    BeginDrawHook();

    glVertexPointer(3, GL_FLOAT, sizeof(vertex), GetOffset(&(vertices[0]), 0));
    if(hasTexCoords)
    {
        glClientActiveTexture(GL_TEXTURE0);
        glTexCoordPointer(2, GL_FLOAT, sizeof(vertex),
                          GetOffset(&(vertices[0]), 12));
    }
    if(hasNormals)
    {
        glNormalPointer(GL_FLOAT, sizeof(vertex), GetOffset(&(vertices[0]), 20));
    }
    if(hasColors)
    {
        glColorPointer(4, GL_FLOAT, sizeof(vertex), GetOffset(&(vertices[0]), 32));
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    if(hasColors)
        glEnableClientState(GL_COLOR_ARRAY);
    if(hasTexCoords)
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    if(hasNormals)
        glEnableClientState(GL_NORMAL_ARRAY);

}

void SORE_Graphics::GraphicsArray::EndDraw()
{
    if(hasColors)
        glDisableClientState(GL_COLOR_ARRAY);
    if(hasNormals)
        glDisableClientState(GL_NORMAL_ARRAY);
    if(hasTexCoords)
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void SORE_Graphics::GraphicsArray::DrawAll(GLenum type)
{
    glDrawElements(type, indices.size(), GL_UNSIGNED_SHORT,
                   GetOffset(&(indices[0]), 0));
}

void SORE_Graphics::GraphicsArray::DrawElements(
    unsigned int numIndices, unsigned short indexOffset, GLenum type)
{
    glDrawElements(type, numIndices, GL_UNSIGNED_SHORT,
                   GetOffset(&(indices[0]), indexOffset*sizeof(unsigned short)));
}

bool SORE_Graphics::GraphicsArray::Empty() const
{
    return indices.empty();
}
