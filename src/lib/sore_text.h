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

#ifndef SORE_TEXT_H
#define SORE_TEXT_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include "sore_color.h"
#include "sore_font.h"
#include "sore_geometrychunk.h"
#include "math/sore_matrix4x4.h"

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
