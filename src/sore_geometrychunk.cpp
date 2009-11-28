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

#include <cstring>

#include "sore_geometrychunk.h"

SORE_Graphics::GeometryChunk::GeometryChunk(
    unsigned short nVertices, unsigned short nIndices)
    : numVertices(nVertices), numIndices(nIndices)
{
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
