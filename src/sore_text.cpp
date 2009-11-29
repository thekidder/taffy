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

#include "sore_text.h"

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
