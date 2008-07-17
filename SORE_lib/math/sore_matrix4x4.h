//$Id$

#ifndef _SORE_MATRIX4X4_H_
#define _SORE_MATRIX4X4_H_

#include <cmath>

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
			
			void Rotate(T rads, unsigned int axis)
			{
				Matrix4<T> rotation = Matrix4<T>::ProduceRotationMatrix(rads, axis);
				*this *= rotation;
			}
			
			void Translate(T x, T y, T z)
			{
				Matrix4<T> translate = Matrix4<T>::ProduceTranslationMatrix(x, y, z);
				*this *= translate;
			}
			
			void Scale(T x, T y, T z)
			{
				Matrix4<T> scale = Matrix4<T>::ProduceScaleMatrix(x, y, z);
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
			
			Matrix4<T>& operator*=(Matrix4<T>& m)
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
			
			static Matrix4<T> ProduceRotationMatrix(T rads, unsigned int axis)
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
			
			static Matrix4<T> ProduceTranslationMatrix(T x, T y, T z)
			{
				Matrix4<T> temp;
				temp.value[12] = x;
				temp.value[13] = y;
				temp.value[14] = z;
				return temp;
			}
			
			static Matrix4<T> ProduceScaleMatrix(T x, T y, T z)
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
}

#endif
