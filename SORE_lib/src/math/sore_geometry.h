// $Id$

#ifndef _SORE_GEOMETRY_H_
#define _SORE_GEOMETRY_H_

namespace SORE_Math
{
	template<class T>
			struct Rect
	{
		Vector2<T> topLeft, bottomRight;
		Rect(Vector2<T> one, Vector2<T> two)
		{
			if(one.x<two.x)
			{
				topLeft = one;
				bottomRight = two;
			}
			else
			{
				topLeft = two;
				bottomRight = one;
			}
			assert(one.x!=two.x || one.y!=two.y);
			assert(one.y<two.y);
		}
		Rect() : topLeft(static_cast<T>(0),static_cast<T>(0)), bottomRight(static_cast<T>(1),static_cast<T>(1))
		{}
	};
}

#endif
