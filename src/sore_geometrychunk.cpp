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

#include <fstream>

#include "sore_util.h"
#include "sore_geometrychunk.h"

namespace SORE_Graphics
{
    GeometryChunk::GeometryChunk(SORE_Graphics::Texture2D* texture,
                                 SORE_Math::Rect<float> bounds,
                                 SORE_Math::Rect<float> texCoords,
                                 const Color& color)
        : vertices(0), texCoords(0), colors(0), opaque(false), c(color), tex(0),
          shader(0)
    {
        setup(bounds, texCoords);
        tex = texture;
    }

    void GeometryChunk::setup(SORE_Math::Rect<float> bounds,
        SORE_Math::Rect<float> texCoordRect)
    {
        vertices = new float[3 * 4];
        texCoords = new float[2 * 4];
        colors = new float[4 * 4];

        indices = new unsigned short[6];
        vertices[0]  = bounds.topLeft[0];
        vertices[1]  = bounds.topLeft[1];
        vertices[2]  = 0.0;

        vertices[3]  = bounds.topLeft[0];
        vertices[4]  = bounds.bottomRight[1];
        vertices[5]  = 0.0;

        vertices[6]  = bounds.bottomRight[0];
        vertices[7]  = bounds.topLeft[1];
        vertices[8]  = 0.0;

        vertices[9]  = bounds.bottomRight[0];
        vertices[10] = bounds.bottomRight[1];
        vertices[11] = 0.0;

        texCoords[0] = texCoordRect.topLeft[0];
        texCoords[1] = texCoordRect.topLeft[1];

        texCoords[2] = texCoordRect.topLeft[0];
        texCoords[3] = texCoordRect.bottomRight[1];

        texCoords[4] = texCoordRect.bottomRight[0];
        texCoords[5] = texCoordRect.topLeft[1];

        texCoords[6] = texCoordRect.bottomRight[0];
        texCoords[7] = texCoordRect.bottomRight[1];

        SetColor(c);

        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 2;
        indices[4] = 1;
        indices[5] = 3;

        numVertices = 4;
        numIndices = 6;
    }

    GeometryChunk::GeometryChunk(const GeometryChunk& gc) :
        vertices(0), texCoords(0), colors(0), indices(0), opaque(gc.opaque),
        c(gc.c), primitiveType(gc.primitiveType), numVertices(gc.numVertices),
        numIndices(gc.numIndices), tex(gc.tex), shader(gc.shader)
    {
        vertices = new float[numVertices*3];
        texCoords = new float[numVertices*2];
        colors = new float[numVertices*4];
        indices = new unsigned short[numIndices];

        memcpy(vertices, gc.vertices, numVertices*3*sizeof(float));
        memcpy(texCoords, gc.texCoords, numVertices*2*sizeof(float));
        memcpy(colors, gc.colors, numVertices*4*sizeof(float));
        memcpy(indices, gc.indices, numIndices*sizeof(unsigned short));
    }

    GeometryChunk::~GeometryChunk()
    {
        delete[] vertices;
        delete[] texCoords;
        delete[] colors;
        delete[] indices;
    }

    const Color& GeometryChunk::GetColor() const
    {
        return c;
    }

    void GeometryChunk::SetColor(const Color& color)
    {
        c = color;
        for(unsigned int i=0;i<4*4;i+=4)
        {
            memcpy(colors + i, color.GetColor(), 4 * sizeof(float));
        }
    }

    void GeometryChunk::SetTexCoords(SORE_Math::Rect<float> texCoordRect)
    {
        texCoords[0] = texCoordRect.topLeft[0];
        texCoords[1] = texCoordRect.topLeft[1];

        texCoords[2] = texCoordRect.topLeft[0];
        texCoords[3] = texCoordRect.bottomRight[1];

        texCoords[4] = texCoordRect.bottomRight[0];
        texCoords[5] = texCoordRect.topLeft[1];

        texCoords[6] = texCoordRect.bottomRight[0];
        texCoords[7] = texCoordRect.bottomRight[1];
    }

    bool GeometryChunk::IsOpaque() const
    {
        return opaque;
    }

    bool GeometryChunk::HasTexture() const
    {
        return texCoords!=0;
    }

    bool GeometryChunk::HasColor() const
    {
        return colors!=0;
    }

    const float * GeometryChunk::Vertices() const
    {
        return vertices;
    }

    const float * GeometryChunk::Colors() const
    {
        return colors;
    }

    const unsigned short * GeometryChunk::Indices() const
    {
        return indices;
    }

    const float* GeometryChunk::TexCoords() const
    {
        return texCoords;
    }

    unsigned int GeometryChunk::NumVertices() const
    {
        return numVertices;
    }

    unsigned int GeometryChunk::NumIndices() const
    {
        return numIndices;
    }

    const SORE_Graphics::Texture2D* GeometryChunk::GetTexture() const
    {
        return tex;
    }

    void GeometryChunk::SetTexture(SORE_Graphics::Texture2D* texture)
    {
        tex = texture;
    }

    const SORE_Graphics::GLSLShader* GeometryChunk::GetShader() const
    {
        return shader;
    }

    void GeometryChunk::SetShader(SORE_Graphics::GLSLShader* shad)
    {
        shader = shad;
    }

    void ApplyTransform(const SORE_Math::Matrix4<float>& transform,
                                            render_list_owned& list)
    {
        render_list_owned::iterator it;
        for(it = list.begin(); it != list.end(); ++it)
        {
            it->first *= transform;
        }
    }

    render_list GetRenderList(render_list_owned& list)
    {
        render_list_owned::iterator it;
        render_list newList;//(list.size());
        for(it = list.begin(); it != list.end(); ++it)
        {
            assert(it->second);
            newList.push_back(std::make_pair(&it->first, it->second));
        }
        return newList;
    }
}
