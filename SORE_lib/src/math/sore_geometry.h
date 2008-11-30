// $Id$

#ifndef _SORE_GEOMETRY_H_
#define _SORE_GEOMETRY_H_

#include "sore_vector2.h"

namespace SORE_Math
{
	template<class T>
		struct Rect
		{
			Vector2<T> topLeft, bottomRight;
			Rect(Vector2<T> one, Vector2<T> two)
			{
				if(one[0]<two[0])
				{
					topLeft = one;
					bottomRight = two;
				}
				else
				{
					topLeft = two;
					bottomRight = one;
				}
				assert(one[0]!=two[0] || one[1]!=two[1]);
				assert(one[1]<two[1]);
			}
		Rect() : topLeft(static_cast<T>(0),static_cast<T>(0)), bottomRight(static_cast<T>(1),static_cast<T>(1))
			{}
		};
}

#endif
