/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#include <sore_color.h>

#include <algorithm>

namespace SORE_Graphics
{
    Color::Color(float r, float g, float b, float a)
    {
        color[0] = r;
        color[1] = g;
        color[2] = b;
        color[3] = a;
    }

    Color::Color(float all)
    {
        color[0] = all;
        color[1] = all;
        color[2] = all;
        color[3] = all;
    }

    const float* Color::GetColor() const
    {
        return color;
    }

    SORE_Math::Vector4<float> Color::GetColorVector() const
    {
        SORE_Math::Vector4<float> c(color[0], color[1], color[2], color[3]);
        return c;
    }

    float Color::GetComponent(ColorComponent c) const
    {
        return color[c];
    }

    void Color::SetComponent(ColorComponent c, float v)
    {
        color[c] = v;
    }

    Color& Color::operator+=(const Color& c)
    {
        for(unsigned int i=0;i<4;++i)
        {
            color[i] = color[i]+c.color[i];
        }
        return *this;
    }

    Color& Color::operator-=(const Color& c)
    {
        for(unsigned int i=0;i<4;++i)
        {
            color[i] = color[i]-c.color[i];
        }
        return *this;
    }

    Color& Color::operator*=(const Color& c)
    {
        for(unsigned int i=0;i<4;++i)
        {
            color[i] = color[i]*c.color[i];
        }
        return *this;
    }

    Color& Color::operator/=(const Color& c)
    {
        for(unsigned int i=0;i<4;++i)
        {
            color[i] = color[i]/c.color[i];
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

    Color operator*(const Color& lhs, const Color& rhs)
    {
        Color temp = lhs;
        return temp *= rhs;
    }

    Color operator/(const Color& lhs, const Color& rhs)
    {
        Color temp = lhs;
        return temp /= rhs;
    }
}
