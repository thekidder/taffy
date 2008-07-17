// $Id$

#ifndef _SORE_VECTOR4_H_
#define _SORE_VECTOR4_H_

#include <cassert>

#include "math/sore_vector3.h"

namespace SORE_Math
{
	template<class T>
			class Vector4
	{
		public:
			Vector4()
			{
				value[0] = T(0);
				value[1] = T(0);
				value[2] = T(0);
				value[3] = T(0);
			}
			Vector4(T x, T y, T z, T w)
			{
				value[0] = x;
				value[1] = y;
				value[2] = z;
				value[3] = w;
			}
			
			Vector4(const Vector3<T>& v, T v4)
			{
				value[0] = v[0];
				value[1] = v[1];
				value[2] = v[2];
				value[3] = v4;
			}
			
			Vector4(const Vector4<T>& v)
			{
				value[0] = v.value[0];
				value[1] = v.value[1];
				value[2] = v.value[2];
				value[3] = v.value[3];
			}
			
			void Set(T x, T y, T z, T w)
			{
				value[0] = x;
				value[1] = y;
				value[2] = z;
				value[3] = w;
			}
			
			T* GetValue()
			{
				return value;
			}
			
			T& operator[] (int index)
			{
				if(index<0 || index>3)
				{
					assert("Index out of range!");
					return value[0];
				}
				return value[index];
			}

			T dot(Vector4<T>& v)
			{
				T temp = v[0]*value[0] + v[1]*value[1] + v[2]*value[2] + v[3]*value[3];
				return temp;
			}
			
			Vector4<T>& operator+=(Vector4<T> v)
			{
				value[0] += v.value[0];
				value[1] += v.value[1];
				value[2] += v.value[2];
				value[3] += v.value[3];
				return *this;
			}
			
			Vector4<T>& operator*=(Vector4<T> v)
			{
				value[0] *= v.value[0];
				value[1] *= v.value[1];
				value[2] *= v.value[2];
				value[3] *= v.value[3];
				return *this;
			}
			
			Vector4<T>& operator/=(Vector4<T> v)
			{
				value[0] /= v.value[0];
				value[1] /= v.value[1];
				value[2] /= v.value[2];
				value[3] /= v.value[3];
				return *this;
			}
			
			//template<class OP>
			Vector4<T>& operator+=(T v)
			{
				value[0] += v;
				value[1] += v;
				value[2] += v;
				value[3] += v;
				return *this;
			}
			//template<class OP>
			Vector4<T>& operator*=(T v)
			{
				value[0] *= v;
				value[1] *= v;
				value[2] *= v;
				value[3] *= v;
				return *this;
			}
			
			
			//template<class OP>
			Vector4<T>& operator/=(T num)
			{
				value[0] /= num;
				value[1] /= num;
				value[2] /= num;
				value[3] /= num;
				return *this;
			}
			
			/*ector4<T> cross(Vector4<T>& v)
			{
				Vector4<T> temp;
				temp[0] = value[1]*v.value[2] - value[2]*v.value[1];
				temp[1] = value[2]*v.value[0] - value[0]*v.value[2];
				temp[2] = value[0]*v.value[1] - value[1]*v.value[0];
				return temp;
			}*/
			
			double Magnitude()
			{
				return sqrt(pow(value[0],2)+pow(value[1],2)+pow(value[2],2)+pow(value[3],2));
			}
			
			Vector4 Normalize()
			{
				double mag = Magnitude();
				Vector4<T> temp(static_cast<T>(value[0]/mag),static_cast<T>(value[1]/mag),static_cast<T>(value[2]/mag),static_cast<T>(value[3]/mag));
				return temp;
			}
				
		protected:
			T value[4];
	};
	
	template<class T>
			double distance(Vector4<T> one, Vector4<T> two)
	{
		return sqrt((one[0]-two[0])*(one[0]-two[0])+(one[1]-two[1])*(one[1]-two[1])+(one[2]-two[2])*(one[2]-two[2]) + (one[3]-two[3])*(one[3]-two[3]));
	}
	
	template<class T>
			Vector4<T> operator-(Vector4<T> v)
	{
		Vector4<T> temp(-v[0],-v[1],-v[2], -v[3]);
		return temp;
	}
	
	template<class T>
			Vector4<T> operator+(Vector4<T> v1, Vector4<T> v2)
	{
		Vector4<T> r = v1;
		r += v2;
		return r;
	}
	
	template<class T>
			Vector4<T> operator-(Vector4<T> v1, Vector4<T> v2)
	{
		Vector4<T> r = v1;
		r += -v2;
		return r;
	}
	
	template<class T>
			Vector4<T> operator/(Vector4<T> v1, T& num)
	{
		Vector4<T> r = v1;
		r /= num;
		return r;
	}
	
	template<class T>
			Vector4<T> operator*(Vector4<T> v1, T num)
	{
		Vector4<T> r = v1;
		r *= num;
		return r;
	}
	
	template<class T>
			Vector4<T> operator*(T num, Vector4<T> v1)
	{
		return v1*num;
	}
}

#endif
