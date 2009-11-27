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

#include "sore_renderable.h"

SORE_Graphics::Renderable::Renderable(
    GeometryChunkPtr g, ShaderPtr s, Texture2DPtr tex,
    TransformationPtr trans, geometry_layer l, blend_mode b)
    : geometry(g), shader(s), texture(tex), transformation(trans),
      layer(l), blending(b), cachedDepth(0.0f), sortKey(0)
{
    CalculateDepth();
    CalculateSortKey();
}

GeometryChunkPtr SORE_Graphics::Renderable::GetGeometryChunk() const
{
    return geometry;
}

void SORE_Graphics::Renderable::SetShader(ShaderPtr s)
{
    shader = s;
    CalculateSortKey();
}

ShaderPtr SORE_Graphics::Renderable::GetShader() const
{
    return shader;
}

void SORE_Graphics::Renderable::SetTexture(TexturePtr t)
{
    texture = t;
    CalculateSortKey();
}

Texture2DPtr SORE_Graphics::Renderable::GetTexture() const
{
    return texture;
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

TransformationPtr SORE_Graphics::Renderable::GetTransform() const
{
    return transformation;
}

int64 SORE_Graphics::Renderable::GetSortKey() const
{
    return sortKey;
}

void SORE_Graphics::Renderable::CalculateDepth()
{
}

void SORE_Graphics::Renderable::CalculateSortKey()
{
}
