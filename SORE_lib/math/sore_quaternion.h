//$Id$

#ifndef _SORE_QUATERNION_H_
#define _SORE_QUATERNION_H_

#include <cmath>

#include "sore_vector3.h"
#include "sore_matrix4x4.h"

namespace SORE_Math
{
	template<class T>
			class Quaternion
	{
		public:
			Quaternion() : real(static_cast<T>(0.0))
			{
			}
			
			Quaternion(T r, Vector3<T> i) : real(r), imaginary(i)
			{
			}
			
			Quaternion<T>& operator*=(Quaternion<T>& q)
			{
				Quaternion(real*q.real - imaginary.dot(q.imaginary), real*q.imaginary + q.real*imaginary + imaginary.cross(q.imaginary));
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
				values[ 0] = real*real + imaginary[0]*imaginary[0] - imaginary[1]*imaginary[1] - imaginary[2]*imaginary[2];
				values[ 1] = 2*imaginary[0]*imaginary[1] - 2*real*imaginary[2];
				values[ 2] = 2*imaginary[0]*imaginary[2] + 2*real*imaginary[1];
				values[ 3] = 0;
				values[ 4] = 2*imaginary[0]*imaginary[1] + 2*real*imaginary[2];
				values[ 5] = real*real - imaginary[0]*imaginary[0] + imaginary[1]*imaginary[1] - imaginary[2]*imaginary[2];;
				values[ 6] = 2*imaginary[1]*imaginary[2] - 2*real*imaginary[0];
				values[ 7] = 0;
				values[ 8] = 2*imaginary[0]*imaginary[2] + 2*real*imaginary[1];
				values[ 9] = 2*imaginary[1]*imaginary[2] + 2*real*imaginary[0];
				values[10] = real*real - imaginary[0]*imaginary[0] - imaginary[1]*imaginary[1] + imaginary[2]*imaginary[2];
				values[11] = 0;
				values[12] = 0;
				values[13] = 0;
				values[14] = 0;
				values[15] = real*real + imaginary[0]*imaginary[0] + imaginary[1]*imaginary[1] + imaginary[2]*imaginary[2];
			}
			
			static Quaternion<T> GetRotation(T rads, Vector3<T> axis)
			{
				Vector3<T> temp = axis * static_cast<T>(sin(rads)/static_cast<T>(2.0));
				Quaternion<T> result(cos(rads)/static_cast<T>(2.0), temp); 
				return result;
			}
		private:
			T real;
			Vector3<T> imaginary;
	};
}

#endif
