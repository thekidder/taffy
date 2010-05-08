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

#include <sore_renderable.h>

SORE_Graphics::Renderable::Renderable() : cachedDepth(0.0f), sortKey(0)
{
    textures = TextureStatePtr(new TextureState());
    uniforms = UniformStatePtr(new UniformState());
}

SORE_Graphics::Renderable::Renderable(
    GeometryChunkPtr g, GLSLShaderPtr s, TransformationPtr trans, 
    geometry_layer l, blend_mode b)
    : geometry(g), shader(s), transformation(trans),
      layer(l), blending(b), cachedDepth(0.0f), sortKey(0)
{
    textures = TextureStatePtr(new TextureState());
    uniforms = UniformStatePtr(new UniformState());

    CalculateDepth();
    CalculateSortKey();
}

void SORE_Graphics::Renderable::SetGeometryChunk(GeometryChunkPtr g)
{
    geometry = g;
    CalculateDepth();
}

SORE_Graphics::GeometryChunkPtr SORE_Graphics::Renderable::GetGeometryChunk() const
{
    return geometry;
}

void SORE_Graphics::Renderable::SetShader(GLSLShaderPtr s)
{
    shader = s;
    CalculateSortKey();
}

SORE_Graphics::GLSLShaderPtr SORE_Graphics::Renderable::GetShader() const
{
    return shader;
}

void SORE_Graphics::Renderable::AddTexture(
    const std::string& samplerName, Texture2DPtr t)
{
    textures->AddTexture(samplerName, t);
    CalculateSortKey();
}

const SORE_Graphics::TextureState& SORE_Graphics::Renderable::GetTextures() const
{
    return *textures;
}

void SORE_Graphics::Renderable::MulitplyTransform(TransformationPtr t)
{
    *transformation *= *t;
    CalculateDepth();
    CalculateSortKey();
}

void SORE_Graphics::Renderable::SetTransform(TransformationPtr t)
{
    transformation = t;
    CalculateDepth();
    CalculateSortKey();
}

SORE_Graphics::TransformationPtr SORE_Graphics::Renderable::GetTransform() const
{
    return transformation;
}

void SORE_Graphics::Renderable::SetBlendMode(blend_mode b)
{
    blending = b;
    CalculateSortKey();
}

SORE_Graphics::blend_mode SORE_Graphics::Renderable::GetBlendMode() const
{
    return blending;
}

void SORE_Graphics::Renderable::SetLayer(geometry_layer l)
{
    layer = l;
    CalculateSortKey();
}

SORE_Graphics::geometry_layer SORE_Graphics::Renderable::GetLayer() const
{
    return layer;
}

SORE_Graphics::UniformState& SORE_Graphics::Renderable::Uniforms()
{
    return *uniforms;
}

const SORE_Graphics::UniformState& SORE_Graphics::Renderable::Uniforms() const
{
    return *uniforms;
}

int SORE_Graphics::Renderable::GetSortKey() const
{
    return sortKey;
}

void SORE_Graphics::Renderable::SetProjection(const ProjectionInfo& pi)
{
    proj = pi;
    CalculateSortKey();
}

void SORE_Graphics::Renderable::CalculateDepth()
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
        pos = *transformation * pos;
        if(pos[2] < minZ)
            minZ = pos[2];
    }
    cachedDepth = minZ;
}

void SORE_Graphics::Renderable::CalculateSortKey()
{
    const unsigned int keyLen = 32;
    const unsigned int depthBits = 11;
    unsigned int depth;

    float z = 0.0f;

    switch(proj.type)
    {
    case ORTHO:
    case ORTHO2D:
        z = (1<<depthBits) *
            (cachedDepth - proj.znear) / (proj.zfar - proj.znear);
        break;
    case PERSPECTIVE:
        z = 1.0f / cachedDepth;
        z = (proj.znear + proj.zfar) / (proj.zfar - proj.znear) +
            cachedDepth * (-2.0f * proj.zfar * proj.znear) /
            (proj.zfar - proj.znear);
        z *= (1<<depthBits);
        break;
    case NONE:
    default:
        break;
    }
    depth = static_cast<int>(z);
    if(blending == BLEND_OPAQUE)
        depth = (1<<depthBits) - depth;

    sortKey = 0;
    sortKey |= (layer << (keyLen - 3));
    sortKey |= (blending << (keyLen - 2 - 3));
    sortKey |= (depth & 0xFFF) << (keyLen - 2 - 3 - 12);
    if(shader)
        sortKey |= (shader->GetHandle() << (keyLen - 2 - 3 - 12 - 6));
    sortKey |= ((textures->GetSortKey() % 512)); //9 bits for texture
}

bool SORE_Graphics::operator<(const SORE_Graphics::Renderable& one,
                              const SORE_Graphics::Renderable& two)
{
    return one.GetSortKey() < two.GetSortKey();
}
