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

#include <sore_assettypes.h>
#include <sore_matrix4x4.h>
#include <sore_geometrychunk.h>
#include <sore_screeninfo.h>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>

namespace SORE_Graphics
{
    class RenderState;

    typedef unsigned long long uint64;
    typedef boost::shared_ptr<SORE_Math::Matrix4<float> > MatrixPtr;

    class SORE_EXPORT Renderable
    {
    public:
        Renderable(GeometryChunkPtr geometry_, MatrixPtr modelMatrix_, SORE_Resource::MaterialPtr material_);        
        Renderable(const Renderable& r, SORE_Resource::MaterialPtr material_);

        void SetTexture(const std::string& samplerName, SORE_Resource::Texture2DPtr texture);
        template<typename T>
        void SetUniform(const std::string& name, const T& value)
        {
            material->SetUniform(name, value);
        }

        const GeometryChunkPtr GetGeometryChunk() const { return geometry; }
        const SORE_Resource::MaterialPtr GetMaterial() const { return material; }

        MatrixPtr GetModelMatrix() { return modelMatrix; }
        const MatrixPtr GetModelMatrix() const { return modelMatrix; }

        int GetSortKey() const { return sortKey; }

        void ClearKeywords();
        void AddKeyword(const std::string& keyword);
        bool HasKeyword(const std::string& keyword) const { return keywords.find(keyword) != keywords.end(); }
    private:
        friend class SORE_Graphics::RenderState;

        void CalculateDepth() const;
        void CalculateSortKey() const;

        GeometryChunkPtr geometry;
        MatrixPtr modelMatrix;
        SORE_Resource::MaterialPtr material;

        mutable float cachedDepth;
        mutable int sortKey;

        boost::unordered_set<std::string> keywords;
    };

    bool operator<(const Renderable& one, const Renderable& two);
}

#endif
