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
//

#ifndef  __SORE_MATH_H__
#define  __SORE_MATH_H__

namespace SORE_Math
{
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
			
			void Log(unsigned int severity)
			{
				ENGINE_LOG(severity, "(%fi, %fj,%fk)",value[0],value[1],value[2]);
			}
				
		protected:
			T value[3];
	};
	
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
	
	typedef Vector3D<double> Vector3Dd;
	typedef Vector3D<float>  Vector3Df;
	typedef Vector3D<int>    Vector3Di;
}

#endif /*__SORE_MATH_H__*/
