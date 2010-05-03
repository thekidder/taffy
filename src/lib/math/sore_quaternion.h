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

#ifndef _SORE_QUATERNION_H_
#define _SORE_QUATERNION_H_

#include <cmath>

#include <sore_vector3.h>
#include <sore_matrix4x4.h>

namespace SORE_Math
{
	template<typename T>
			class SORE_EXPORT Quaternion
	{
		public:
			Quaternion() : real(static_cast<T>(0.0))
			{
				imaginary[0] = 1.0;
			}
			
			void Assign(T r, Vector3<T> i)
			{
				imaginary = i;
				real = r;
			}
			
			Quaternion(T rads, Vector3<T> axis) //create from axis and angle
			{
				*this = GetRotation(rads, axis);
			}
			
			Quaternion<T>& operator*=(Quaternion<T>& q)
			{
				T r = real*q.real - imaginary.dot(q.imaginary);
				Vector3<T> i = real*q.imaginary + q.real*imaginary + imaginary.cross(q.imaginary);
				real = r;
				imaginary = i;
				return *this;
			}
			
			void Rotate(T rads, Vector3<T> axis)
			{
				Quaternion<T> temp = GetRotation(rads, axis);
				*this *= temp;
			}
			
			Matrix4<T> GetMatrix()
			{
				T values[16];
				values[ 0] = 1.0f - 2.0f * ( imaginary[1] * imaginary[1] + imaginary[2] * imaginary[2] );
				values[ 1] = 2.0f * (imaginary[0] * imaginary[1] + imaginary[2] * real);
				values[ 2] = 2.0f * (imaginary[0] * imaginary[2] - imaginary[1] * real);
				values[ 3] = 0.0f;
	
				values[ 4] = 2.0f * ( imaginary[0] * imaginary[1] - imaginary[2] * real );
				values[ 5] = 1.0f - 2.0f * ( imaginary[0] * imaginary[0] + imaginary[2] * imaginary[2] );
				values[ 6] = 2.0f * (imaginary[2] * imaginary[1] + imaginary[0] * real );
				values[ 7] = 0.0f;

				values[ 8] = 2.0f * ( imaginary[0] * imaginary[2] + imaginary[1] * real );
				values[ 9] = 2.0f * ( imaginary[1] * imaginary[2] - imaginary[0] * real );
				values[10] = 1.0f - 2.0f * ( imaginary[0] * imaginary[0] + imaginary[1] * imaginary[1] );
				values[11] = 0.0f;

				values[12] = 0.0f;
				values[13] = 0.0f;
				values[14] = 0.0f;
				values[15] = 1.0f;

				return Matrix4<T>(values);
			}
			
			static Quaternion<T> GetRotation(T rads, Vector3<T> axis)
			{
				Vector3<T> temp = axis * static_cast<T>(  sin(rads/2.0)  );
				T r = cos(rads/2.0);
				Quaternion<T> result;
				result.Assign(r, temp); 
				return result;
			}
		private:
			T real;
			Vector3<T> imaginary;
	};
	
	template<typename T>
			Quaternion<T> operator*(Quaternion<T> q1, Quaternion<T> q2)
	{
		Quaternion<T> temp = q1;
		q1 *= q2;
		return q1;
	}
}

#endif
