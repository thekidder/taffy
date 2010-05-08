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

#ifndef SORE_TEXT_H
#define SORE_TEXT_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <sore_color.h>
#include <sore_font.h>
#include <sore_geometrychunk.h>
#include <sore_matrix4x4.h>

namespace SORE_Graphics
{
    class SORE_EXPORT Text
    {
    public:
        Text(SORE_Font::Font& f, unsigned int h, const std::string& initalText,
             const Color& c = White);
        virtual ~Text();

        void UpdateText(const std::string& newText, const Color& c = White);
        void SetTransform(const SORE_Math::Matrix4<float>& transform);
        void TrimToWidth(unsigned int width, size_t start);

        const SORE_Math::Matrix4<float>& GetTransform();
        std::vector<Renderable> GetGeometry() const;
        //following two functions are only valid if text is not scaled or rotated
        unsigned int GetWidth() const;
        unsigned int GetHeight() const;

        //returns number of characters in text
        size_t GetLength() const;
        size_t GetIndex(float xpos) const;
        const std::string& GetText() const;
    private:
        void Update();

        unsigned int height;
        unsigned int width;
        std::string text;
        Color color;
        SORE_Font::Font& face;

        SORE_Math::Matrix4<float> overallTransform;
        std::vector<Renderable> geometry;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
