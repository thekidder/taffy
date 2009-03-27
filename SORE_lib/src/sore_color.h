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

#ifndef SORE_COLOR_H
#define SORE_COLOR_H

namespace SORE_Graphics
{
	enum ColorComponent {RED = 0, GREEN, BLUE, ALPHA};

	//represents a four-component color with an internal type of float
	class Color
	{
	public:
		Color();
		Color(float r, float g, float b, float a);

		const float* GetColor() const;
		float GetComponent(ColorComponent c);
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
