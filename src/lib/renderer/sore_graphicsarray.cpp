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

#include <sore_graphicsarray.h>
#include <sore_logger.h>

SORE_Graphics::GraphicsArray::GraphicsArray(geometry_type type, bool t, bool c, bool n)
  : hasTexCoords(t), hasNormals(n), hasColors(c), type(type)
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

void SORE_Graphics::GraphicsArray::AddObject(GeometryChunkPtr geometry)
{
    size_t oldSize = vertices.size();

    vertices.resize(vertices.size() + geometry->NumVertices());
    memcpy(&vertices[oldSize], geometry->GetVertices(),
           geometry->NumVertices()*sizeof(vertex));

    //copy vertices into VBO, taking into account index renumbering
    indices.resize(indices.size() + geometry->NumIndices());
    for(unsigned short j=0;j<geometry->NumIndices();++j)
    {
        unsigned short index = geometry->GetIndex(j) + oldSize;
        indices.push_back(index);
    }
}

void SORE_Graphics::GraphicsArray::UpdateObject(GeometryChunkPtr geometry)
{
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

void SORE_Graphics::GraphicsArray::EndDraw() const
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

bool SORE_Graphics::GraphicsArray::HasRoomFor(size_t numIndices, size_t numVertices) const
{
    return !(NumIndices() + numIndices > std::numeric_limits<unsigned short>::max() ||
             NumVertices() + numVertices > std::numeric_limits<unsigned short>::max());
}

size_t SORE_Graphics::GraphicsArray::NumIndices() const
{
    return indices.size();
}

size_t SORE_Graphics::GraphicsArray::NumVertices() const
{
    return vertices.size();
}
