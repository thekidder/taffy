/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//$Id$

#ifndef SORE_TEXT_H
#define SORE_TEXT_H

#include "sore_color.h"
#include "sore_font.h"
#include "sore_geometrychunk.h"
#include "math/sore_matrix4x4.h"

namespace SORE_Graphics
{
	class Text
	{
	public:
		Text(SORE_Font::Font& f, unsigned int h, const std::string& initalText, const Color& c = White);

		void UpdateText(const std::string& newText, const Color& c = White);
		void SetTransform(const SORE_Math::Matrix4<float>& transform);
		void TrimToWidth(unsigned int width, size_t start);

		const SORE_Math::Matrix4<float>& GetTransform();
		render_list GetGeometry() const;
		//following two functions are only valid if text is not scaled or rotated
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;

		//returns number of characters in text
		size_t GetLength() const;
		size_t GetIndex(float xpos) const;
		const std::string& GetText() const;
	private:
		void Update();

		Color color;
		SORE_Font::Font& face;
		std::string text;
		unsigned int height;
		unsigned int width;

		SORE_Math::Matrix4<float> overallTransform;

		std::vector<std::pair<SORE_Math::Matrix4<float>, GeometryChunk*> > geometry;
	};
}

#endif
