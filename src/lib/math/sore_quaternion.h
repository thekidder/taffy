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

#ifndef _SORE_QUATERNION_H_
#define _SORE_QUATERNION_H_

#define _USE_MATH_DEFINES
#include <cmath>

#include <sore_vector3.h>
#include <sore_matrix4x4.h>

namespace SORE_Math
{
	template<typename T>
			class SORE_EXPORT Quaternion
	{
		public:
			Quaternion() : x(T(1.0)), y(T(0.0)), z(T(0.0)), w(T(0.0))
			{
			}
			
			void Assign(T x_, T y_, T z_, T w_)
			{
				x = x_;
                y = y_;
                z = z_;
				w = w_;
			}
			
			Quaternion(T rads, Vector3<T> axis) //create from axis and angle
			{
				*this = GetRotation(rads, axis);
			}
			
			Quaternion<T>& operator*=(Quaternion<T>& q)
			{
                Quaternion<T> temp;
				temp.x = w*q.x - x*q.w - y*q.z - z*q.y;
                temp.y = w*q.y - x*q.z + y*q.w + z*q.x;
                temp.z = w*q.z + x*q.y + y*q.x - z*q.w;
                temp.w = w*q.w + x*q.x - y*q.y + z*q.z;

                *this = temp;

                if(fabs(x*x + y*y + z*z + w*w - T(1.0)) > 0.0001)
                {
                    // normalize
                    T magnitude = sqrt(x*x + y*y + z*z + w*w);
                    x /= magnitude;
                    y /= magnitude;
                    z /= magnitude;
                    w /= magnitude;
                }

				return *this;
			}
			
			void Rotate(T rads, Vector3<T> axis)
			{
				Quaternion<T> temp = GetRotation(rads, axis);
				*this = temp * *this;
			}
			
			Matrix4<T> GetMatrix() const
			{
				T values[16];
				values[ 0] = T(1.0) - T(2.0) * ( y * y + z * z );
				values[ 1] = T(2.0) * (x * y + z * w);
				values[ 2] = T(2.0) * (x * z - y * w);
				values[ 3] = T(0.0);
	
				values[ 4] = T(2.0) * ( x * y - z * w );
				values[ 5] = T(1.0) - T(2.0) * ( x * x + z * z );
				values[ 6] = T(2.0) * (z * y - x * w );
				values[ 7] = T(0.0);

				values[ 8] = T(2.0) * ( x * z + y * w );
				values[ 9] = T(2.0) * ( y * z + x * w );
				values[10] = T(1.0) - T(2.0) * ( x * x + y * y );
				values[11] = T(0.0);

				values[12] = T(0.0);
				values[13] = T(0.0);
				values[14] = T(0.0);
				values[15] = T(1.0);

				return Matrix4<T>(values);
			}
			
			static Quaternion<T> GetRotation(T rads, Vector3<T> axis)
			{
				Quaternion result;
				result.x = axis[0] * sin(rads / T(2.0));
                result.y = axis[1] * sin(rads / T(2.0));
                result.z = axis[2] * sin(rads / T(2.0));
                result.w = cos(rads / T(2.0));
				return result;
			}
		private:
			T x, y, z, w;
	};
	
	template<typename T>
			Quaternion<T> operator*(Quaternion<T> q1, Quaternion<T> q2)
	{
		Quaternion<T> temp = q1;
		temp *= q2;
		return temp;
	}
}

#endif
