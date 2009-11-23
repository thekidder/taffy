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

#ifndef SORE_GEOMETRYCHUNK_H
#define SORE_GEOMETRYCHUNK_H

#include <boost/function.hpp>

#include <map>
#include "sore_color.h"
#include "sore_shaders.h"
#include "sore_texture.h"
#include "math/sore_matrix4x4.h"
#include "math/sore_geometry.h"

/**
   @author Adam Kidder <thekidder@gmail.com>
*/
namespace SORE_Graphics
{
    static SORE_Math::Rect<float> defaultTexCoords(0.0f, 1.0f, 0.0f, 1.0f);

    class SORE_EXPORT GeometryChunk
    {
    public:
        GeometryChunk(Texture2D* texture, GLSLShader* shad,
                      SORE_Math::Rect<float> bounds,
                      SORE_Math::Rect<float> texCoords = defaultTexCoords,
                      const Color& color = White);
        //produces a deep copy
        GeometryChunk(const GeometryChunk& gc);
        ~GeometryChunk();

        const Color& GetColor() const;
        void SetColor(const Color& color);

        const float* TexCoords() const;
        void SetTexCoords(SORE_Math::Rect<float> texCoordRect);

        float GetDepth() const;
        bool HasTexture() const;
        bool HasColor() const;
        bool IsOpaque() const;
        const float* Vertices() const;
        const float* Colors() const;
        const unsigned short* Indices() const;

        unsigned int NumVertices() const;
        unsigned int NumIndices() const;

        const SORE_Graphics::Texture2D* GetTexture() const;
        const SORE_Graphics::GLSLShader* GetShader() const;
        void SetTexture(SORE_Graphics::Texture2D* texture);
        void SetShader(SORE_Graphics::GLSLShader* shad);
    private:
        void setup(SORE_Math::Rect<float> bounds,
                   SORE_Math::Rect<float> texCoordRect);

        //geometry
        float* vertices;
        float* texCoords;
        float* colors;

        unsigned short* indices;
        bool opaque;
        Color c;

        unsigned int primitiveType;
        unsigned int numVertices, numIndices;

        SORE_Graphics::Texture2D* tex;
        SORE_Graphics::GLSLShader* shader;

    };

    typedef std::vector<std::pair<const SORE_Math::Matrix4<float>*,
                                  const GeometryChunk *> >
    render_list;

    typedef std::vector<std::pair<SORE_Math::Matrix4<float>, GeometryChunk *> >
    render_list_owned;

    void SORE_EXPORT ApplyTransform(const SORE_Math::Matrix4<float>& transform,
                                    render_list_owned& list);
    render_list SORE_EXPORT GetRenderList(render_list_owned& list);

    const int SORT_LESS    = -1;
    const int SORT_EQUAL   =  0;
    const int SORT_GREATER =  1;
}
#endif
