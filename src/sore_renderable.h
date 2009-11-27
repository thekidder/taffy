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

#ifndef SORE_RENDERABLE_H
#define SORE_RENDERABLE_H

#include <boost/shared_ptr.hpp>

#include "sore_geometrychunk.h"

namespace SORE_Graphics
{
    enum geometry_layer
    {
        LAYER0,
        LAYER1,
        LAYER2,
        LAYER3,
        LAYER4,
        LAYER5,
        LAYER6,
        LAYER7
    };

    enum blend_mode
    {
        OPAQUE,
        ADDITIVE,
        SUBTRACTIVE,
        UNUSED
    };

    typedef unsigned long long int64;

    typedef boost::shared_ptr<GeometryChunk> GeometryChunkPtr;
    typedef boost::shared_ptr<GLSLShader> ShaderPtr;
    typedef boost::shared_ptr<Texture2D> Texture2DPtr;
    typedef boost::shared_ptr<SORE_Math::Matrix4<float> > TransformationPtr;

    class Renderable
    {
    public:
        Renderable(GeometryChunkPtr g, ShaderPtr s, Texture2DPtr tex,
                   TransformationPtr trans, geometry_layer l, blend_mode b);

        void SetShader(ShaderPtr s);
        ShaderPtr GetShader() const;

        void SetTexture(TexturePtr t);
        Texture2DPtr GetTexture() const;

        void MulitplyTransform(TransformationPtr t);
        void SetTransform(TransformationPtr t);
        TransformationPtr GetTransform() const;

        int64 GetSortKey() const;

    private:
        void CalculateDepth();
        void CalculateSortKey();

        GeometryChunkPtr geometry;
        ShaderPtr shader;
        Texture2DPtr texture;
        TransformationPtr transformation;
        geometry_layer layer;
        blend_mode blending;

        float cachedDepth;

        int64 sortKey;
    };
}

#endif
