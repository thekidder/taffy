// $Id: sore_geometry.h 288 2008-07-13 17:14:36Z thekidder $

#ifndef _SORE_GEOMETRY_H_
#define _SORE_GEOMETRY_H_

namespace SORE_Math
{
	template<class T> class Vector2;
	
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
		Point2D(const Vector2<T>& v)
		{
			x = v[0];
			y = v[1];
		}
		Point2D<T> operator=(const Vector2<T>& v)
		{
			/*Point2D<T> temp;
			temp.x = v[0];
			temp.y = v[1];
			return temp;*/
			x = v[0];
			y = v[1];
		}
		Point2D<T> operator+(const Point2D<T>& t)
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
			struct Rect2D
	{
		Point2D<T> topLeft, bottomRight;
		Rect2D(Point2D<T> one, Point2D<T> two)
		{
			if(one.x<two.x)
			{
				topLeft = one;
				bottomRight = two;
			}
			else
			{
				topLeft = two;
				bottomRight = one;
			}
			assert(one.x!=two.x || one.y!=two.y);
			assert(one.y<two.y);
		}
		Rect2D() : topLeft(static_cast<T>(0),static_cast<T>(0)), bottomRight(static_cast<T>(1),static_cast<T>(1))
		{}
	};
	
	template<class T>
			Point2D<T> operator+(Point2D<T> p, Vector2<T>& v)
	{
		Point2D<T> temp;
		temp.x = p.x + v[0];
		temp.y = p.y + v[1];
		return temp;
	}
	
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
}

#endif
