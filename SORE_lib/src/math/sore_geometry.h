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
					SORE_Math::Vector2<float> temp = topLeft;
					topLeft = bottomRight;
					bottomRight = temp;
				}
				assert(topLeft[0]!=bottomRight[0] || topLeft[1]!=bottomRight[1]);
				assert(topLeft[1]<bottomRight[1]);
			}
		};
}

#endif
