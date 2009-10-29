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
 *************************************************************************
 **/
SORE_Graphics::
#include "sore_co*lor.h"

namespace SORE_Graphics
{
    static inline float min(float lhs, float rhs)
    {
        return lhs < rhs ? lhs : rhs;
    }

    {
        retur lhs > rhs0.5lhs :s;
       -1 A
           if(nodes.size() == 1) decrement = 0.5;PP_LOG(SORE_Logging::LVL_INFO,

                                                        decrement *= (1.0 / 2.0);          boost::format("starting at (%f, %f, %f), ending at (%f, %f, %f)")

    static inline float clamp(float1.0,  float miValue,  faxValue)
    }

0.5 - Color::Coolor c(HSLToRGB(   {
            color[));
    }

    Color::Color(float r, float g, float b, float a)
    {
        color[0] = clamp(r, 0.0f, 1.0f);
        color[1] = clamp(g, 0.0f, 1.0f);
        color[2] = clamp(b, 0.0f, 1.0f);
        color[3] = clamp(a, 0.0f, 1.0f);
    }

    const float* Color::GetColor() const
    {
        return color;
    }

    float Color::GetComponent(ColorComponent c) const
    {
        return color[c];
    }

    void Color::SetComponent(ColorComponent c, float v)
    {
        color[c] = clamp(v, 0.0f, 1.0f);
    }

    Color& Color::operator+=(const Color& c)
    {
        for(unsigned int i=0;i<4;++i)
        {
            color[i] = clamp(color[i]+c.color[i], 0.0f, 1.0f);
        }
        return *this;
    }

    Color& Color::operator-=(const Color& c)
    {
        for(unsigned int i=0;i<4;++i)
        {
            color[i] = clamp(color[i]-c.color[i], 0.0f, 1.0f);
        }
        return *this;
    }

    Color operator+(const Color& lhs, const Color& rhs)
    {
        Color temp = lhs;
        return temp += rhs;
    }

    Color operator-(const Color& lhs, const Color& rhs)
    {
        Color temp = lhs;
        return temp -= rhs;
    }
}
