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
* THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
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

#include <cstring>
#include <limits>

#include <sore_geometrychunk.h>
#include <sore_logger.h>

SORE_Graphics::GeometryChunk::GeometryChunk(
    unsigned int nVertices, unsigned int nIndices, GLenum type)
    : numVertices(nVertices), numIndices(nIndices), type(type)
{
    if(nIndices > std::numeric_limits<unsigned short>::max())
        ENGINE_LOG(SORE_Logging::LVL_ERROR, "Too many indices in geometry chunk");
    if(nVertices > std::numeric_limits<unsigned short>::max())
        ENGINE_LOG(SORE_Logging::LVL_ERROR, "Too many vertices in geometry chunk");
    if((type == GL_TRIANGLES && nIndices % 3 != 0) ||
        (type == GL_LINES     && nIndices % 2 != 0) ||
        (type == GL_QUADS     && nIndices % 4 != 0))
        ENGINE_LOG(SORE_Logging::LVL_ERROR, "Number of indices is invalid");
    data = new vertex[numVertices];
    indices = new unsigned short[numIndices];
}

SORE_Graphics::GeometryChunk::GeometryChunk(const GeometryChunk& o)
{
    Init(o);
}

SORE_Graphics::GeometryChunk::~GeometryChunk()
{
    delete[] data;
    delete[] indices;
}

SORE_Graphics::GeometryChunk& SORE_Graphics::GeometryChunk::operator=(
    const GeometryChunk& o)
{
    if(this == &o)
        return *this;
    delete[] data;
    delete[] indices;
    Init(o);
    return *this;
}

void SORE_Graphics::GeometryChunk::Init(const GeometryChunk& o)
{
    type = o.type;
    numVertices = o.numVertices;
    numIndices = o.numIndices;
    data = new vertex[numVertices];
    indices = new unsigned short[numIndices];
    memcpy(data, o.data, numVertices*sizeof(vertex));
    memcpy(indices, o.indices, numIndices*sizeof(unsigned short));
}

void SORE_Graphics::GeometryChunk::SetColor(const Color& c)
{
    for(unsigned short i = 0; i < numVertices; ++i)
    {
        data[i].r = c.GetComponent(RED);
        data[i].g = c.GetComponent(GREEN);
        data[i].b = c.GetComponent(BLUE);
        data[i].a = c.GetComponent(ALPHA);
    }
}

SORE_Graphics::vertex* const SORE_Graphics::GeometryChunk::GetVertices()
{
    return data;
}

SORE_Graphics::vertex& SORE_Graphics::GeometryChunk::GetVertex(unsigned short i)
{
    assert(i < numVertices);
    return data[i];
}

GLenum SORE_Graphics::GeometryChunk::Type() const
{
    return type;
}

unsigned short SORE_Graphics::GeometryChunk::NumVertices() const
{
    return numVertices;
}

unsigned short SORE_Graphics::GeometryChunk::NumIndices() const
{
    return numIndices;
}

unsigned short* const SORE_Graphics::GeometryChunk::GetIndices()
{
    return indices;
}

unsigned short& SORE_Graphics::GeometryChunk::GetIndex(unsigned short i)
{
    assert(i < numIndices);
    return indices[i];
}
