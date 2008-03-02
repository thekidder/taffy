//
// C++ Interface: sore_math
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
// $Id$


#ifndef  __SORE_MATH_H__
#define  __SORE_MATH_H__

#include <cmath>

namespace SORE_Math
{
	const float PI_180 = (float)M_PI/180.0f;
	
	template<class T> class Vector2D;
	
	template<class T>
			struct Point3D
	{
		Point3D(T _x, T _y, T _z)
		{
			x = _x;
			y = _y;
			z = _z;
		}
		Point3D()
		{
			x = T(0);
			y = T(0);
			z = T(0);
		}
		
		Point3D<T> operator+(Point3D<T>& t)
		{
			Point3D<T> temp;
			temp.x = x + t.x;
			temp.y = y + t.y;
			temp.z = z + t.z;
			return temp;
		}
		Point3D<T> operator-(Point3D<T>& t)
		{
			Point3D<T> temp;
			temp.x = x - t.x;
			temp.y = y - t.y;
			temp.z = z - t.z;
			return temp;
		}
		T x,y,z;
	};
	
	template<class T>
			struct Point2D
	{
		Point2D(T _x, T _y)
		{
			x = _x;
			y = _y;
		}
		Point2D()
		{
			x = T(0);
			y = T(0);
		}
		Point2D(Vector2D<T>& v)
		{
			x = v[0];
			y = v[1];
		}
		Point2D<T> operator=(Vector2D<T>& v)
		{
			Point2D<T> temp;
			temp.x = v[0];
			temp.y = v[1];
			return temp;
		}
		Point2D<T> operator+(Point2D<T>& t)
		{
			Point2D<T> temp;
			temp.x = x + t.x;
			temp.y = y + t.y;
			return temp;
		}
		Point2D<T> operator-(Point2D<T>& t)
		{
			Point2D<T> temp;
			temp.x = x - t.x;
			temp.y = y - t.y;
			return temp;
		}
		T x,y;
	};
	
	template<class T>
			class Vector3D
	{
		public:
			Vector3D()
			{
				value[0] = T(0);
				value[1] = T(0);
				value[2] = T(0);
			}
			Vector3D(T x, T y, T z)
			{
				value[0] = x;
				value[1] = y;
				value[2] = z;
			}
			Vector3D(Point3D<T> p)
			{
				value[0] = p.x;
				value[1] = p.y;
				value[2] = p.z;
			}
			
			Vector3D(const Vector3D<T>& v)
			{
				value[0] = v.value[0];
				value[1] = v.value[1];
				value[2] = v.value[2];
			}
			
			void Set(T x, T y, T z)
			{
				value[0] = x;
				value[1] = y;
				value[2] = z;
			}
			
			T* GetValue()
			{
				return value;
			}
			
			T operator[] (int index) const
			{
				if(index<0 || index>2) return T(0.0);
				return value[index];
			}

			T dot(Vector3D<T>& v)
			{
				T temp = v[0]*value[0] + v[1]*value[1] + v[2]*value[2];
				return temp;
			}
			
			Vector3D<T>& operator+=(Vector3D<T> v)
			{
				value[0] += v.value[0];
				value[1] += v.value[1];
				value[2] += v.value[2];
				return *this;
			}
			
			Vector3D<T>& operator*=(Vector3D<T> v)
			{
				value[0] *= v.value[0];
				value[1] *= v.value[1];
				value[2] *= v.value[2];
				return *this;
			}
			
			Vector3D<T>& operator/=(Vector3D<T> v)
			{
				value[0] /= v.value[0];
				value[1] /= v.value[1];
				value[2] /= v.value[2];
				return *this;
			}
			
			//template<class OP>
					Vector3D<T>& operator+=(T v)
			{
				value[0] += v;
				value[1] += v;
				value[2] += v;
				return *this;
			}
			//template<class OP>
					Vector3D<T>& operator*=(T v)
			{
				value[0] *= v;
				value[1] *= v;
				value[2] *= v;
				return *this;
			}
			
			
			//template<class OP>
					Vector3D<T>& operator/=(T num)
			{
				value[0] /= num;
				value[1] /= num;
				value[2] /= num;
				return *this;
			}
			
			Vector3D<T> cross(Vector3D<T>& v)
			{
				Point3D<T> p;
				p.x = value[1]*v.value[2] - value[2]*v.value[1];
				p.y = value[2]*v.value[0] - value[0]*v.value[2];
				p.z = value[0]*v.value[1] - value[1]*v.value[0];
				Vector3D<T> temp(p);
				return temp;
			}
			double Magnitude()
			{
				return sqrt(pow(value[0],2)+pow(value[1],2)+pow(value[2],2));
			}
			
			Vector3D Normalize()
			{
				double mag = Magnitude();
				Vector3D<T> temp(value[0]/mag,value[1]/mag,value[2]/mag);
				return temp;
			}
				
		protected:
			T value[3];
	};
	
	template<class T>
			class Vector2D
	{
		public:
			Vector2D()
			{
				value[0] = T(0);
				value[1] = T(0);
			}
			Vector2D(T x, T y)
			{
				value[0] = x;
				value[1] = y;
			}
			Vector2D(Point2D<T> p)
			{
				value[0] = p.x;
				value[1] = p.y;
			}
			
			Vector2D(const Vector2D<T>& v)
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
			
			T operator[] (int index) const
			{
				if(index<0 || index>1) return T(0.0);
				return value[index];
			}

			T dot(Vector2D<T>& v)
			{
				T temp = v[0]*value[0] + v[1]*value[1];
				return temp;
			}
			
			Vector2D<T>& operator+=(Vector2D<T> v)
			{
				value[0] += v.value[0];
				value[1] += v.value[1];
				return *this;
			}
			Vector2D<T>& operator-=(Vector2D<T> v)
			{
				value[0] -= v.value[0];
				value[1] -= v.value[1];
				return *this;
			}
			
			Vector2D<T>& operator*=(Vector2D<T> v)
			{
				value[0] *= v.value[0];
				value[1] *= v.value[1];
				return *this;
			}
			
			Vector2D<T>& operator/=(Vector2D<T> v)
			{
				value[0] /= v.value[0];
				value[1] /= v.value[1];
				return *this;
			}
			
			//template<class OP>
			Vector2D<T>& operator+=(T v)
			{
				value[0] += v;
				value[1] += v;
				return *this;
			}
			Vector2D<T>& operator-=(T v)
			{
				value[0] -= v;
				value[1] -= v;
				return *this;
			}
			//template<class OP>
			Vector2D<T>& operator*=(T v)
			{
				value[0] *= v;
				value[1] *= v;
				return *this;
			}
			
			
			//template<class OP>
			Vector2D<T>& operator/=(T num)
			{
				value[0] /= num;
				value[1] /= num;
				return *this;
			}
			
			double Magnitude()
			{
				return sqrt(pow(value[0],2)+pow(value[1],2));
			}
			
			Vector2D Normalize()
			{
				double mag = Magnitude();
				if(mag==0.0) return Vector2D(0.0,0.0);
				Vector2D<T> temp(value[0]/mag,value[1]/mag);
				return temp;
			}
		protected:
			T value[2];
	};
	
	template<class T>
			double distance(Vector3D<T> one, Vector3D<T> two)
	{
		return sqrt((one[0]-two[0])*(one[0]-two[0])+(one[1]-two[1])*(one[1]-two[1])+(one[2]-two[2])*(one[2]-two[2]));
	}
	
	template<class T>
			Vector3D<T> operator-(Vector3D<T> v)
	{
		Vector3D<T> temp(-v[0],-v[1],-v[2]);
		return temp;
	}
	
	template<class T>
			Vector3D<T> operator+(Vector3D<T> v1, Vector3D<T> v2)
	{
		Vector3D<T> r = v1;
		r += v2;
		return r;
	}
	
	template<class T>
			Vector3D<T> operator-(Vector3D<T> v1, Vector3D<T> v2)
	{
		Vector3D<T> r = v1;
		r += -v2;
		return r;
	}
	
	template<class T>
			Vector3D<T> operator/(Vector3D<T> v1, T& num)
	{
		Vector3D<T> r = v1;
		r /= num;
		return r;
	}
	
	template<class T>
			Vector3D<T> operator*(Vector3D<T> v1, T num)
	{
		Vector3D<T> r = v1;
		r *= num;
		return r;
	}
	
	template<class T>
			Vector3D<T> operator*(T num, Vector3D<T> v1)
	{
		return v1*num;
	}
	
	template<class T>
			double distance(Vector2D<T> one, Vector2D<T> two)
	{
		return sqrt((one[0]-two[0])*(one[0]-two[0])+(one[1]-two[1])*(one[1]-two[1]));
	}
	
	template<class T>
			Vector2D<T> operator-(Vector2D<T> v)
	{
		Vector2D<T> temp(-v[0],-v[1]);
		return temp;
	}
	
	template<class T>
			Vector2D<T> operator+(Vector2D<T> v1, Vector2D<T> v2)
	{
		Vector2D<T> r = v1;
		r += v2;
		return r;
	}
	
	template<class T>
			Vector2D<T> operator-(Vector2D<T> v1, Vector2D<T> v2)
	{
		Vector2D<T> r = v1;
		r += -v2;
		return r;
	}
	
	template<class T>
			Vector2D<T> operator/(Vector2D<T> v1, T& num)
	{
		Vector2D<T> r = v1;
		r /= num;
		return r;
	}
	
	template<class T>
			Vector2D<T> operator*(Vector2D<T> v1, T num)
	{
		Vector2D<T> r = v1;
		r *= num;
		return r;
	}
	
	template<class T>
			Vector2D<T> operator*(T num, Vector2D<T> v1)
	{
		return v1*num;
	}

	template<class T>
			bool operator>(Vector2D<T> one, Vector2D<T> two)
	{
		return one.Magnitude() > two.Magnitude();
	}
	
	template<class T>
	Point2D<T> operator+(Point2D<T> p, Vector2D<T>& v)
	{
		Point2D<T> temp;
		temp.x = p.x + v[0];
		temp.y = p.y + v[1];
		return temp;
	}
	
	
	
	typedef Vector3D<double> Vector3Dd;
	typedef Vector3D<float>  Vector3Df;
	typedef Vector3D<int>    Vector3Di;
	typedef Vector2D<double> Vector2Dd;
	typedef Vector2D<float>  Vector2Df;
	typedef Vector2D<int>    Vector2Di;
	
	float Distance3D(float x1, float y1, float z1, float x2, float y2, float z2);
	float Distance2D(float x1, float y1, float x2, float y2);
	float Distance2DS(float x1, float y1, float x2, float y2); //distance squared
	
	template<class T>
			double Distance2D(Point2D<T> one, Point2D<T> two)
	{
		return sqrt((double)(two.x-one.x)*(two.x-one.x) + (two.y-one.y)*(two.y-one.y));
	}
	
	template<class T>
			T Distance2DS(Point2D<T> one, Point2D<T> two)
	{
		return (two.x-one.x)*(two.x-one.x) + (two.y-one.y)*(two.y-one.y);
	}
	
	bool DoubleEquals(double value, double compare, double epsilon);
}

#endif /*__SORE_MATH_H__*/
