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

#ifndef SORE_BATCH_H
#define SORE_BATCH_H

#include "sore_graphicsarray.h"
#include "sore_renderable.h"
#include "sore_texture.h"
#include "sore_renderstate.h"
#include "sore_screeninfo.h"
#include "sore_shaders.h"

namespace SORE_Graphics
{
    struct geometry_entry
    {
        GraphicsArray* geometry;
        unsigned int offset;
        unsigned int num;
        GLenum type;
    };

    class RenderBatch
    {
    public:
        //sets command to either NONE or BIND_VBO
        RenderBatch(const geometry_entry& geometry,
                    const RenderState& state, bool bindVBO = false);

        void AddIndices(unsigned int numIndices);
        //returns number of polygons rendered
        unsigned int Render();
    private:
        GraphicsArray* geometry;
        unsigned int numberIndices;
        unsigned int indexOffset;
        GLenum type;
        bool bind;

        RenderState state;
    };
}

#endif
