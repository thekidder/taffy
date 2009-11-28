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
    GeometryChunkPtr g, GLSLShaderPtr s, Texture2DPtr tex,
    TransformationPtr trans, geometry_layer l, blend_mode b)
    : geometry(g), shader(s), texture(tex), transformation(trans),
      layer(l), blending(b), cachedDepth(0.0f), sortKey(0)
{
    CalculateDepth();
    CalculateSortKey();
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

void SORE_Graphics::Renderable::SetTexture(Texture2DPtr t)
{
    texture = t;
    CalculateSortKey();
}

SORE_Graphics::Texture2DPtr SORE_Graphics::Renderable::GetTexture() const
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

SORE_Graphics::TransformationPtr SORE_Graphics::Renderable::GetTransform() const
{
    return transformation;
}

SORE_Graphics::int64 SORE_Graphics::Renderable::GetSortKey() const
{
    return sortKey;
}

void SORE_Graphics::Renderable::CalculateDepth()
{
}

void SORE_Graphics::Renderable::CalculateSortKey()
{
}

bool SORE_Graphics::operator<(const SORE_Graphics::Renderable& one,
                              const SORE_Graphics::Renderable& two)
{
    return one.GetSortKey() < two.GetSortKey();
}
