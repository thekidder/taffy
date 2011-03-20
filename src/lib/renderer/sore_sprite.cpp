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

#include <sore_sprite.h>

SORE_Graphics::Renderable SORE_Graphics::MakeSprite(
        SORE_Math::Rect<float> bounds,
        SORE_Math::Rect<float> texCoords,
        float z,
        Texture2DPtr texture,
        GLSLShaderPtr shader,
        blend_mode b)
{
    GeometryChunkPtr g(new GeometryChunk(4, 6, GL_TRIANGLES));
    unsigned short* const indices = g->GetIndices();
    vertex* const vertices = g->GetVertices();

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 1;
    indices[5] = 3;

    vertices[0].x = bounds.topLeft[0];
    vertices[0].y = bounds.topLeft[1];
    vertices[0].z = z;
    vertices[0].tex0i = texCoords.topLeft[0];
    vertices[0].tex0j = texCoords.topLeft[1];

    vertices[1].x = bounds.topLeft[0];
    vertices[1].y = bounds.bottomRight[1];
    vertices[1].z = z;
    vertices[1].tex0i = texCoords.topLeft[0];
    vertices[1].tex0j = texCoords.bottomRight[1];

    vertices[2].x = bounds.bottomRight[0];
    vertices[2].y = bounds.topLeft[1];
    vertices[2].z = z;
    vertices[2].tex0i = texCoords.bottomRight[0];
    vertices[2].tex0j = texCoords.topLeft[1];

    vertices[3].x = bounds.bottomRight[0];
    vertices[3].y = bounds.bottomRight[1];
    vertices[3].z = z;
    vertices[3].tex0i = texCoords.bottomRight[0];
    vertices[3].tex0j = texCoords.bottomRight[1];

    g->SetColor(White);

    TransformationPtr transformation(new SORE_Math::Matrix4<float>());
    Renderable r(g, shader, transformation, b);
    if(texture)
        r.AddTexture("texture", texture);
    return r;
}

SORE_Graphics::Renderable SORE_Graphics::MakePointSprite(
        SORE_Math::Vector3<float> position,
        float size,
        Texture2DPtr texture,
        GLSLShaderPtr shader,
        blend_mode b)
{
    GeometryChunkPtr g(new GeometryChunk(1, 1, GL_POINTS));
    unsigned short* const indices = g->GetIndices();
    vertex* const vertices = g->GetVertices();

    indices[0] = 0;

    vertices[0].x = position[0];
    vertices[0].y = position[1];
    vertices[0].z = position[2];
   
    g->SetColor(White);

    TransformationPtr transformation(new SORE_Math::Matrix4<float>());
    Renderable r(g, shader, transformation, b);
    if(texture)
        r.AddTexture("texture", texture);
    r.Uniforms().SetVariable("pointSize", size);
    return r;
}