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
