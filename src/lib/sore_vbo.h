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

#ifndef SORE_VBO_H
#define SORE_VBO_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <sore_graphicsarray.h>

namespace SORE_Graphics
{
    /**
       @author Adam Kidder <thekidder@gmail.com>
       For use with dynamic (change every frame) indexed VBOs
    */
    class SORE_EXPORT VBO : public GraphicsArray
    {
    public:
        VBO(bool s, bool t = false, bool c = false, bool n = false);
        ~VBO();

        virtual void Build();
    private:
        virtual void BeginDrawHook();
        virtual void* GetOffset(void* pointer, unsigned int offset);

        GLuint vbo, vboIndices;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
