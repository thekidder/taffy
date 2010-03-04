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

#include "sore_batch.h"

SORE_Graphics::RenderBatch::RenderBatch(
    const geometry_entry& geometry,
    const RenderState& state, bool bindVBO)
    : geometry(geometry.geometry), bind(bindVBO), numberIndices(geometry.num),
      indexOffset(geometry.offset), type(geometry.type), state(state)
{
}

void SORE_Graphics::RenderBatch::AddIndices(unsigned int numIndices)
{
    numberIndices += numIndices;
}

unsigned int SORE_Graphics::RenderBatch::Render()
{
    state.Apply();
    if(bind)
    {
        geometry->BeginDraw();
    }
    if(geometry)
    {
        unsigned int trisPerPoly;
        if(type == GL_POINTS)
            trisPerPoly = 1;
        else if(type == GL_LINES)
            trisPerPoly = 2;
        else if(type == GL_TRIANGLES)
            trisPerPoly = 3;
        else
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Unsupported primitive type");
        geometry->DrawElements(numberIndices, indexOffset, type);
        return numberIndices/trisPerPoly;
    }
    else
    {
        return 0;
    }
}

