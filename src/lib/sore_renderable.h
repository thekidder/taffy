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

#include "math/sore_matrix4x4.h"
#include "sore_geometrychunk.h"
#include "sore_texturestate.h"
#include "sore_screeninfo.h"
#include "sore_shaders.h"
#include "sore_uniformstate.h"

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
        BLEND_OPAQUE,
        BLEND_ADDITIVE,
        BLEND_SUBTRACTIVE,
        BLEND_UNUSED
    };

    typedef unsigned long long int64;
    typedef boost::shared_ptr<SORE_Math::Matrix4<float> > TransformationPtr;

    class SORE_EXPORT Renderable
    {
    public:
        Renderable();
        Renderable(GeometryChunkPtr g, GLSLShaderPtr s, TransformationPtr trans, 
            geometry_layer l, blend_mode b);

        void SetGeometryChunk(GeometryChunkPtr g);
        GeometryChunkPtr GetGeometryChunk() const;

        void SetShader(GLSLShaderPtr s);
        GLSLShaderPtr GetShader() const;

        void AddTexture(const std::string& samplerName, Texture2DPtr t);
        const TextureState& GetTextures() const;

        void MulitplyTransform(TransformationPtr t);
        void SetTransform(TransformationPtr t);
        TransformationPtr GetTransform() const;

        void SetBlendMode(blend_mode b);
        blend_mode GetBlendMode() const;

        void SetLayer(geometry_layer l);
        geometry_layer GetLayer() const;

        UniformState& Uniforms();
        const UniformState& Uniforms() const;

        int GetSortKey() const;

        void SetProjection(const ProjectionInfo& pi);
    private:
        void CalculateDepth();
        void CalculateSortKey();

        GeometryChunkPtr geometry;
        GLSLShaderPtr shader;
        TextureStatePtr textures;
        TransformationPtr transformation;
        geometry_layer layer;
        blend_mode blending;
        UniformStatePtr uniforms;

        float cachedDepth;

        int sortKey;

        ProjectionInfo proj;
    };

    bool operator<(const Renderable& one, const Renderable& two);
}

#endif
