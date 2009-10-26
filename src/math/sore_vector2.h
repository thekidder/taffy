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

#ifndef _SORE_VECTOR2_H_
#define _SORE_VECTOR2_H_

#include <cassert>
#include <cmath>

#include "../sore_dll.h"

namespace SORE_Math
{
	template<typename T>
	class Vector2
	{
	public:
		Vector2()
		{
			value[0] = T(0);
			value[1] = T(0);
		}
		Vector2(T x, T y)
		{
			value[0] = x;
			value[1] = y;
		}
			
		Vector2(const Vector2<T>& v)
		{
			value[0] = v.value[0];
			value[1] = v.value[1];
		}
			
		void Set(T x, T y)
		{
			value[0] = x;
			value[1] = y;
		}
			
		T* GetValue()
		{
			return value;
		}
			
		T& operator[] (int index)
		{
			if(index<0 || index>1) 
			{
				assert("Index out of range!");
				return value[0];
			}
			return value[index];
		}

		const T& operator[] (int index)  const
		{
			if(index<0 || index>1) 
			{
				assert("Index out of range!");
				return value[0];
			}
			return value[index];
		}

		T dot(const Vector2<T>& v)
		{
			T temp = v[0]*value[0] + v[1]*value[1];
			return temp;
		}
			
		Vector2<T>& operator+=(Vector2<T> v)
		{
			value[0] += v.value[0];
			value[1] += v.value[1];
			return *this;
		}
		Vector2<T>& operator-=(Vector2<T> v)
		{
			value[0] -= v.value[0];
			value[1] -= v.value[1];
			return *this;
		}
			
		Vector2<T>& operator*=(Vector2<T> v)
		{
			value[0] *= v.value[0];
			value[1] *= v.value[1];
			return *this;
		}
			
		Vector2<T>& operator/=(Vector2<T> v)
		{
			value[0] /= v.value[0];
			value[1] /= v.value[1];
			return *this;
		}
			
		Vector2<T>& operator+=(T v)
		{
			value[0] += v;
			value[1] += v;
			return *this;
		}

		Vector2<T>& operator-=(T v)
		{
			value[0] -= v;
			value[1] -= v;
			return *this;
		}

		Vector2<T>& operator*=(T v)
		{
			value[0] *= v;
			value[1] *= v;
			return *this;
		}
						
		Vector2<T>& operator/=(T num)
		{
			value[0] /= num;
			value[1] /= num;
			return *this;
		}
			
		T Magnitude()
		{
			return sqrt(value[0]*value[0]+value[1]*value[1]);
		}
			
		Vector2<T> Normalize()
		{
			T mag = Magnitude();
			if(mag==0.0) return Vector2(0.0,0.0);
			Vector2<T> temp(value[0]/mag,value[1]/mag);
			return temp;
		}
	protected:
		T value[2];
	};
	
	template<typename T>
	double distance(Vector2<T> one, Vector2<T> two)
	{
		return sqrt((one[0]-two[0])*(one[0]-two[0])+(one[1]-two[1])*(one[1]-two[1]));
	}
	
	template<typename T>
	Vector2<T> operator-(Vector2<T> v)
	{
		Vector2<T> temp(-v[0],-v[1]);
		return temp;
	}
	
	template<typename T>
	Vector2<T> operator+(Vector2<T> v1, Vector2<T> v2)
	{
		Vector2<T> r = v1;
		r += v2;
		return r;
	}
	
	template<typename T>
	Vector2<T> operator-(Vector2<T> v1, Vector2<T> v2)
	{
		Vector2<T> r = v1;
		r += -v2;
		return r;
	}
	
	template<typename T>
	Vector2<T> operator/(Vector2<T> v1, T& num)
	{
		Vector2<T> r = v1;
		r /= num;
		return r;
	}
	
	template<typename T>
	Vector2<T> operator*(Vector2<T> v1, T num)
	{
		Vector2<T> r = v1;
		r *= num;
		return r;
	}
	
	template<typename T>
	Vector2<T> operator*(const T& num, const Vector2<T>& v1)
	{
		return v1*num;
	}

	template<typename T>
	bool operator>(Vector2<T> one, Vector2<T> two)
	{
		return one.Magnitude() > two.Magnitude();
	}
}

#endif
