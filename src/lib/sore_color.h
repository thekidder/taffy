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
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
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

#ifndef SORE_COLOR_H
#define SORE_COLOR_H

#include <sore_dll.h>
#include <sore_vector4.h>

namespace SORE_Graphics
{
    enum ColorComponent {RED = 0, GREEN, BLUE, ALPHA};

    //represents a four-component color with an internal type of float
    class SORE_EXPORT Color
    {
    public:
        Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

        SORE_Math::Vector4<float> GetColorVector() const;
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

    static const Color White (1.0f, 1.0f, 1.0f, 1.0f);
    static const Color Black (0.0f, 0.0f, 0.0f, 1.0f);

    static const Color Red   (1.0f, 0.0f, 0.0f, 1.0f);
    static const Color Green (0.0f, 1.0f, 0.0f, 1.0f);
    static const Color Blue  (0.0f, 0.0f, 1.0f, 1.0f);

    static const Color Yellow(1.0f, 1.0f, 0.0f, 1.0f);
}

#endif
