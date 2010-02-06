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

#include "sore_sprite.h"

SORE_Graphics::Renderable SORE_Graphics::MakeSprite(
        SORE_Math::Rect<float> bounds,
        SORE_Math::Rect<float> texCoords,
        float z,
        Texture2DPtr texture,
        GLSLShaderPtr shader,
        geometry_layer l,
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
    Renderable r(g, shader, transformation, l, b);
    if(texture)
        r.AddTexture("texture", texture);
    return r;
}
