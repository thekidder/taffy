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

#ifndef _SORE_GEOMETRY_H_
#define _SORE_GEOMETRY_H_

#include <sore_vector2.h>

namespace SORE_Math
{
	template<typename T>
		struct Rect
		{
			Vector2<T> topLeft, bottomRight;
			Rect(Vector2<T> one, Vector2<T> two)
			{
				swap();
			}
			Rect(T left, T right, T top, T bottom)
			{
				topLeft = Vector2<T>(left, top);
				bottomRight = Vector2<T>(right, bottom);
				swap();
			}
			Rect() : topLeft(static_cast<T>(0),static_cast<T>(0)), 
				bottomRight(static_cast<T>(1),static_cast<T>(1))
			{}
		protected:
			void swap()
			{				
				if(topLeft[0]>bottomRight[0])
				{
					SORE_Math::Vector2<T> temp = topLeft;
					topLeft = bottomRight;
					bottomRight = temp;
				}
				assert(topLeft[0]!=bottomRight[0] || topLeft[1]!=bottomRight[1]);
				assert(topLeft[1]<bottomRight[1]);
			}
		};
}

#endif
