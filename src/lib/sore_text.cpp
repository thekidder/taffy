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

#include <sore_text.h>

namespace SORE_Graphics
{
    Text::Text(SORE_Font::Font& f, unsigned int h,
               const std::string& initialText, const Color& c)
        : height(h), text(initialText), color(c), face(f)
    {
        Update();
    }

    Text::~Text()
    {
    }

    unsigned int Text::GetWidth() const
    {
        return width;
    }

    size_t Text::GetIndex(float xpos) const
    {
        float currentAdvance = 0.0f;
        const char* str = text.c_str();
        unsigned int i;
        for(i = 0; str[i]; ++i)
        {
            const SORE_Font::CharInfo& c = face.GetCharacter(height, str[i]);

            currentAdvance += c.advance;
            if(currentAdvance > xpos) return i;
        }
        return i;
    }

    unsigned int Text::GetHeight() const
    {
        return height;
    }

    const std::string& Text::GetText() const
    {
        return text;
    }

    size_t Text::GetLength() const
    {
        return text.size();
    }

    std::vector<Renderable> Text::GetGeometry() const
    {
        return geometry;
    }

    void Text::UpdateText(const std::string& newText, const Color& c)
    {
        color = c;
        text = newText;
        Update();
    }

    void Text::SetTransform(const SORE_Math::Matrix4<float>& transform)
    {
        overallTransform = transform;
        Update();
    }

    void Text::TrimToWidth(unsigned int width, size_t start)
    {
        if(start >= text.length()) return;

        unsigned int numChars = text.length();

        while(GetWidth() > width)
        {
            numChars--;
            UpdateText(text.substr(start, numChars));
        }
    }

    const SORE_Math::Matrix4<float>& Text::GetTransform()
    {
        return overallTransform;
    }

    void Text::Update()
    {
        geometry.clear();
        const char* str = text.c_str();
        float currentAdvance = 0.0f;
        for(unsigned int i = 0; str[i]; ++i)
        {
            const SORE_Font::CharInfo& c = face.GetCharacter(height, str[i]);

            //account for characters without any geometry (i.e. space)
            if(c.renderable.GetGeometryChunk())
            {
                //create a shared geometry
                GeometryChunkPtr coloredCharacter(
                    new GeometryChunk(*c.renderable.GetGeometryChunk()));
                coloredCharacter->SetColor(color);

                TransformationPtr m(
                    new SORE_Math::Matrix4<float>(*c.renderable.GetTransform()));
                *m *= SORE_Math::Matrix4<float>::GetTranslation(
                    currentAdvance, 0.0f, 0.0f);
                *m *= overallTransform;

                Renderable r = c.renderable;
                r.SetGeometryChunk(coloredCharacter);
                r.SetTransform(m);

                geometry.push_back(r);
            }
            currentAdvance += c.advance;
        }
        width = static_cast<unsigned int>(currentAdvance);
    }
}
