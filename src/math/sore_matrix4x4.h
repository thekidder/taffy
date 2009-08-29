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
//$Id$

#ifndef _SORE_MATRIX4X4_H_
#define _SORE_MATRIX4X4_H_

#include <cmath>

#include "sore_vector2.h"
#include "sore_vector3.h"
#include "sore_vector4.h"

namespace SORE_Math
{
	const unsigned int AXIS_X = 0;
	const unsigned int AXIS_Y = 1;
	const unsigned int AXIS_Z = 2;
	
	template<class T>
			class Matrix4
	{
		public:
			Matrix4()
			{
				SetIdentity();
			}
			
			Matrix4(T* v)
			{
				for(unsigned int i=0;i<16;++i)
					value[i] = v[i];
			}
			
			void Rotate(T rads, unsigned int axis)
			{
				Matrix4<T> rotation = Matrix4<T>::GetRotation(rads, axis);
				*this *= rotation;
			}
			
			void Rotate(T rads, SORE_Math::Vector3<T> axis)
			{
				Matrix4<T> rotation = Matrix4<T>::GetRotation(rads, axis);
				*this *= rotation;
			}
			
			void Translate(T x, T y, T z)
			{
				Matrix4<T> translate = Matrix4<T>::GetTranslation(x, y, z);
				*this *= translate;
			}

			void SetPosition(T x, T y, T z)
			{
				value[12] = x;
				value[13] = y;
				value[14] = z;
			}
			
			void Scale(T x, T y, T z)
			{
				Matrix4<T> scale = Matrix4<T>::GetScale(x, y, z);
				*this *= scale;
			}
			
			void SetIdentity()
			{
				for(unsigned int i=0;i<16;++i)
				{
					if(i == 0 || i == 5 || i == 10 || i == 15)
						value[i] = static_cast<T>(1.0);
					else
						value[i] = static_cast<T>(0.0);
				}
			}
			
			Matrix4<T>& operator*=(const Matrix4<T>& m)
			{
				T newValues[16];
				for(unsigned int c=0;c<4;++c)
				{
					for(unsigned int r=0;r<4;++r)
					{
						newValues[c + r*4] = value[c + 0]*m.value[0 + r*4] + value[c + 4]*m.value[1 + r*4] + value[c + 8]*m.value[2 + r*4] + value[c + 12]*m.value[3 + r*4];
					}
				}
				//memcpy(&value[0], &newValues[0], sizeof(T)*16);
				for(unsigned int i=0;i<16;++i)
				{
					value[i] = newValues[i];
				}
				return *this;
			}
			
			Matrix4<T> Transpose()
			{
				T newValues[16];
				for(unsigned int c=0;c<4;++c)
				{
					for(unsigned int r=0;r<4;++r)
					{
						newValues[c + r*4] = value[r + c*4];
					}
				}
				return Matrix4<T>(newValues);
			}
			
			bool Normalized()
			{
				float curr = 0.0;
				for(unsigned int c=0;c<4;++c)
				{
					for(unsigned int r=0;r<4;++r)
					{
						curr += value[c + r*4]*value[c + r*4];
					}
					if(fabs(curr-1.0) > 0.001)
						return false;
					curr = 0.0;
				}
				
				for(unsigned int r=0;r<4;++r)
				{
					for(unsigned int c=0;c<4;++c)
					{
						curr += value[c + r*4]*value[c + r*4];
					}
					if(fabs(curr-1.0) > 0.001)
						return false;
					curr = 0.0;
				}
				return true;
			}
			
			static Matrix4<T> GetRotation(T rads, unsigned int axis)
			{
				Matrix4<T> temp;
				switch(axis)
				{
					case AXIS_X:
						temp.value[5 ] =  cos(rads);
						temp.value[6 ] =  sin(rads);
						temp.value[9 ] = -sin(rads);
						temp.value[10] =  cos(rads);
						break;
					case AXIS_Y:
						temp.value[0 ] =  cos(rads);
						temp.value[2 ] =  sin(rads);
						temp.value[8 ] = -sin(rads);
						temp.value[10] =  cos(rads);
						break;
					case AXIS_Z:
						temp.value[0 ] =  cos(rads);
						temp.value[1 ] = -sin(rads);
						temp.value[4 ] =  sin(rads);
						temp.value[5 ] =  cos(rads);
						break;
					default:
						break;
				}
				return temp;
			}
			
			static Matrix4<T> GetRotation(T rads, SORE_Math::Vector3<T> axis)
			{
				axis = axis.Normalize();
				T c = cos(rads);
				T s = sin(rads);
				T raw[16];
				raw[ 0] = axis[0]*axis[0]*(1-c)+c;
				raw[ 1] = axis[1]*axis[0]*(1-c)+axis[2]*s;
				raw[ 2] = axis[0]*axis[2]*(1-c)-axis[1]*s;
				raw[ 3] = 0.0;
				raw[ 4] = axis[0]*axis[1]*(1-c)-axis[2]*s;
				raw[ 5] = axis[1]*axis[1]*(1-c)+c;
				raw[ 6] = axis[1]*axis[2]*(1-c)+axis[0]*s;
				raw[ 7] = 0.0;
				raw[ 8] = axis[0]*axis[2]*(1-c)+axis[1]*s;
				raw[ 9] = axis[1]*axis[2]*(1-c)-axis[0]*s;
				raw[10] = axis[2]*axis[2]*(1-c)+c;
				raw[11] = 0.0;
				raw[12] = 0.0;
				raw[13] = 0.0;
				raw[14] = 0.0;
				raw[15] = 1.0;
				Matrix4<T> temp(raw);
				return temp;
			}
			
			static Matrix4<T> GetTranslation(T x, T y, T z)
			{
				Matrix4<T> temp;
				temp.value[12] = x;
				temp.value[13] = y;
				temp.value[14] = z;
				return temp;
			}
			
			static Matrix4<T> GetScale(T x, T y, T z)
			{
				Matrix4<T> temp;
				temp.value[0 ] = x;
				temp.value[5 ] = y;
				temp.value[10] = z;
				return temp;
			}
			
			T* GetData()
			{
				return &value[0];
			}

			const T* GetData() const
			{
				return &value[0];
			}
			
		protected:
			T value[16];
	};
	
	template<class T>
			Matrix4<T> operator*(Matrix4<T> m1, Matrix4<T> m2)
	{
		Matrix4<T> temp = m1;
		m1 *= m2;
		return m1;
	}
	
	template<class T>
			Vector4<T> operator*(const Matrix4<T>& m1, const Vector4<T>& v1)
	{
		Vector4<T> temp;
		temp[0] = m1.GetData()[ 0]*v1[0] + m1.GetData()[ 4]*v1[1] + m1.GetData()[ 8]*v1[2] + m1.GetData()[12]*v1[3];
		temp[1] = m1.GetData()[ 1]*v1[0] + m1.GetData()[ 5]*v1[1] + m1.GetData()[ 9]*v1[2] + m1.GetData()[13]*v1[3];
		temp[2] = m1.GetData()[ 2]*v1[0] + m1.GetData()[ 6]*v1[1] + m1.GetData()[10]*v1[2] + m1.GetData()[14]*v1[3];
		temp[3] = m1.GetData()[ 3]*v1[0] + m1.GetData()[ 7]*v1[1] + m1.GetData()[11]*v1[2] + m1.GetData()[15]*v1[3];
		return temp;
	}
	
	template<class T>
			Vector3<T> operator*(const Matrix4<T>& m, const Vector3<T>& v)
	{
		Vector4<T> temp(v[0], v[1], static_cast<T>(1), static_cast<T>(1));
		temp = m * temp;
		Vector3<T> result(temp[0], temp[1], temp[2]);
		return result;
	}

	
	template<typename T>
	Vector2<T> operator*(const Matrix4<T>& m, const Vector2<T>& v)
	{
		Vector4<T> temp(v[0], v[1], static_cast<T>(1), static_cast<T>(1));
		temp = m * temp;
		Vector2<T> result(temp[0], temp[1]);
		return result;
	}
}

#endif
