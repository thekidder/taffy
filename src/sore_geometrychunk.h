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

#ifndef SORE_GEOMETRYCHUNK_H
#define SORE_GEOMETRYCHUNK_H

#include <boost/shared_ptr.hpp>

#include "sore_color.h"
#include "sore_dll.h"

/**
   @author Adam Kidder <thekidder@gmail.com>
*/
namespace SORE_Graphics
{
    struct vertex
    {
        float x, y, z;
        float tex0i, tex0j;
        float normx, normy, normz;
        float r, g, b, a;
    };

    class SORE_EXPORT GeometryChunk
    {
    public:
        GeometryChunk(unsigned short nVertices, unsigned short nIndices);
        GeometryChunk(const GeometryChunk& o);
        ~GeometryChunk();

        //sets up vertices to the same color
        void SetColor(const Color& c);

        vertex* const GetVertices();
        vertex& GetVertex(unsigned short i);

        unsigned short* const GetIndices();
        unsigned short& GetIndex(unsigned short i);

        unsigned short NumVertices() const;
        unsigned short NumIndices() const;

        GeometryChunk& operator=(const GeometryChunk& o);

    private:
        void Init(const GeometryChunk& o);

        unsigned short numVertices, numIndices;
        vertex* data;
        unsigned short* indices;

    };

    typedef boost::shared_ptr<GeometryChunk> GeometryChunkPtr;
}

#endif
