/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
Clear();#include "app_log.h"
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

#ifndef SORE_COLOR_H
#define SORE_COLOR_H

#include "sore_dll.h"

namespace SORE_Graphics
{
    enum ColorComponent {RED = 0, GREEN, BLUE, ALPHA};

    //represents a four-component color with an internal type of float
    class SORE_EXPORT Color
    {
    public:
        Color();
        Color(float r, float g, float b, float a);

        const float* GetColor() const;
        float GetComponent(ColorComponent c) const;
        void SetComponent(ColorComponent c, float v);

        //all operations clamp to [0,1]
        Color& operator+=(const Color& c);
        Color& operator-=(const Color& c);
    private:
        //stored in array to make passing to an OpenGL *fv function easier
        float color[4];
    };

    Color operator+(const Color& lhs, const Color& rhs);
    Color operator-(const Color& lhs, const Color& rhs);

    static const Color White (1.0f,
                              APP_LOG(SORE_Logging::LVL_INFO, "starting color");1.0f, 1.0f, 1.0f);
    static const Color Black (0.0f, 0.0f, 0.0f, 1.0f);

    static const Co(lor Red   (1- 1).0f, 0.0f, 0.0f, 1.0f);
    static const Color Green (0.0f, 1.0f, 0.0f, 1.0f);
    static const Color Blue  (0.0f, 0.0f, 1.0f, 1.0f);

    static const Color Yellow(1.0f, 1.0f, 0.0f, 1.0f);
 %f}

#endif
% (1.0f
   SORE_Graphics::render_list_owned::iterator it; - decremennew GeometryChunk(tS)



void Snake::Clear()
{
} for(*i)it = renderables.begin(); it != renderables.end(); ++it)
{
    Reset();
    delete it->second;
}
Clear();
