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

#ifndef SORE_GRAPHICS_ARRAY_H
#define SORE_GRAPHICS_ARRAY_H

#include <vector>

#include "math/sore_matrix4x4.h"
#include "sore_allgl.h"
#include "sore_geometrychunk.h"

namespace SORE_Graphics
{
    /**
       @author Adam Kidder <thekidder@gmail.com>
       Class abstracting the details of Vertex Arrays and VBOs
    */
    class SORE_EXPORT GraphicsArray
    {
    public:
        GraphicsArray(bool t = false, bool c = false, bool n = false);
        virtual ~GraphicsArray();

        virtual void Build() = 0;

        void Clear();
        void AddObject(GeometryChunkPtr geometry,
                       boost::shared_ptr<SORE_Math::Matrix4<float> > transform);

        void BeginDraw();
        void EndDraw();

        void DrawAll(GLenum type);
        void DrawElements(unsigned int numIndices, unsigned short indexOffset, GLenum type);

        bool Empty() const;
    protected:
        virtual void BeginDrawHook() = 0;
        virtual void* GetOffset(void* pointer, unsigned int offset) = 0;

        std::vector<vertex> vertices;
        std::vector<unsigned short> indices;

        bool hasTexCoords, hasNormals, hasColors;
    };
}

#endif //SORE_GRAPHICS_ARRAY_H
