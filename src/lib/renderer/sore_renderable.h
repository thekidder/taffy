/**************************************************************************
 * Copyright 2011 Adam Kidder. All rights reserved.                       *
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

#ifndef SORE_RENDERABLE_H
#define SORE_RENDERABLE_H

#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>

#include <sore_matrix4x4.h>
#include <sore_geometrychunk.h>
#include <sore_texturestate.h>
#include <sore_screeninfo.h>
#include <sore_glslshader.h>
#include <sore_uniformstate.h>

namespace SORE_Graphics
{
    enum blend_mode
    {
        BLEND_ADDITIVE,
        BLEND_SUBTRACTIVE,
        BLEND_OPAQUE,
        BLEND_UNUSED
    };

    typedef unsigned long long int64;
    typedef boost::shared_ptr<SORE_Math::Matrix4<float> > TransformationPtr;

    class SORE_EXPORT Renderable
    {
    public:
        Renderable();
        Renderable(GeometryChunkPtr g, SORE_Resource::GLSLShaderPtr s, TransformationPtr trans,
            blend_mode b);

        void SetGeometryChunk(GeometryChunkPtr g);
        GeometryChunkPtr GetGeometryChunk() const;

        void SetShader(SORE_Resource::GLSLShaderPtr s);
        SORE_Resource::GLSLShaderPtr GetShader() const;

        void AddTexture(const std::string& samplerName, const TextureState::TextureObject& texture);
        void SetTexture(const std::string& samplerName, SORE_Resource::Texture2DPtr texture);

        const TextureState& Textures() const;
        TextureState& Textures();

        void MulitplyTransform(TransformationPtr t);
        void SetTransform(TransformationPtr t);
        TransformationPtr GetTransform() const;

        void SetBlendMode(blend_mode b);
        blend_mode GetBlendMode() const;

        UniformState& Uniforms();
        const UniformState& Uniforms() const;

        int GetSortKey() const;

        void SetProjection(const ProjectionInfo& pi);

        void ClearKeywords();
        void AddKeyword(const std::string& keyword);
        bool HasKeyword(const std::string& keyword) const;
    private:
        void CalculateDepth() const;
        void CalculateSortKey() const;

        GeometryChunkPtr geometry;
        SORE_Resource::GLSLShaderPtr shader;
        TextureState textures;
        TransformationPtr transformation;
        blend_mode blending;
        UniformState uniforms;

        mutable float cachedDepth;
        mutable int sortKey;

        ProjectionInfo proj;

        boost::unordered_set<std::string> keywords;
    };

    bool operator<(const Renderable& one, const Renderable& two);
}

#endif
