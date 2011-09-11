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

#include <sore_material.h>
#include <sore_renderable.h>

SORE_Graphics::Renderable::Renderable(
    GeometryChunkPtr geometry_, 
    MatrixPtr modelMatrix_, 
    SORE_Resource::MaterialPtr material_)
    : geometry(geometry_), modelMatrix(modelMatrix_), material(material_),
      cachedDepth(0.0f), sortKey(0)
{
    CalculateDepth();
    CalculateSortKey();
}

SORE_Graphics::Renderable::Renderable(
    const Renderable& r, 
    SORE_Resource::MaterialPtr material_)
    : geometry(r.geometry), modelMatrix(r.modelMatrix), material(material_),
    cachedDepth(r.cachedDepth), sortKey(r.sortKey), keywords(r.keywords)
{
}

void SORE_Graphics::Renderable::SetTexture(
    const std::string& samplerName, 
    SORE_Resource::Texture2DPtr texture)
{
    material->SetTexture(samplerName, texture);
    CalculateSortKey();
}

void SORE_Graphics::Renderable::CalculateDepth() const
{
    //sort by min-Z
    float minZ = std::numeric_limits<float>::max();
    for(unsigned int i = 0; i < geometry->NumVertices(); ++i)
    {
        SORE_Math::Vector4<float> pos(
            geometry->GetVertex(i).x,
            geometry->GetVertex(i).y,
            geometry->GetVertex(i).z,
            1.0f);
        pos = *GetModelMatrix() * pos;
        if(pos[2] < minZ)
            minZ = pos[2];
    }
    cachedDepth = minZ;
}

void SORE_Graphics::Renderable::CalculateSortKey() const
{
    const unsigned int keyLen = 64;
    const unsigned int depthBits = 32;
    uint64 depth;

    double z = (static_cast<uint64>(1)<<depthBits) * cachedDepth;

    depth = static_cast<uint64>(z);

    sortKey = 0;
    sortKey |= (depth & 0xFFFFFFFF) << 32;
    if(material->shader)
        sortKey |= (static_cast<uint64>((material->shader->GetHandle() % 65536)) << 16);
    sortKey |= ((material->textures.GetSortKey() % 65536)); //16 bits for texture
}


void SORE_Graphics::Renderable::ClearKeywords()
{
    keywords.clear();
}

void SORE_Graphics::Renderable::AddKeyword(const std::string& keyword)
{
    keywords.insert(keyword);
}

bool SORE_Graphics::operator<(const SORE_Graphics::Renderable& one,
                              const SORE_Graphics::Renderable& two)
{
    return one.GetSortKey() < two.GetSortKey();
}
