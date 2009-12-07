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

#ifndef _SORE_VECTOR3_H_
#define _SORE_VECTOR3_H_

#include <cassert>

namespace SORE_Math
{
    template<typename T>
    class  Vector3
    {
    public:
        Vector3()
        {
            value[0] = T(0);
            value[1] = T(0);
            value[2] = T(0);
        }
        Vector3(T x, T y, T z)
        {
            value[0] = x;
            value[1] = y;
            value[2] = z;
        }

        Vector3(const Vector3<T>& v)
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

        const T* GetValue() const
        {
            return value;
        }

        T& operator[] (int index)
        {
            if(index<0 || index>2)
            {
                assert("Index out of range!");
                return value[0];
            }
            return value[index];
        }

        T operator[] (int index) const
        {
            if(index<0 || index>2)
            {
                assert("Index out of range");
                return value[0];
            }
            return value[index];
        }

        T dot(const Vector3<T>& v)
        {
            T temp = v[0]*value[0] + v[1]*value[1] + v[2]*value[2];
            return temp;
        }

        Vector3<T>& operator+=(const Vector3<T>& v)
        {
            value[0] += v.value[0];
            value[1] += v.value[1];
            value[2] += v.value[2];
            return *this;
        }

        Vector3<T>& operator*=(const Vector3<T>& v)
        {
            value[0] *= v.value[0];
            value[1] *= v.value[1];
            value[2] *= v.value[2];
            return *this;
        }

        Vector3<T>& operator/=(const Vector3<T>& v)
        {
            value[0] /= v.value[0];
            value[1] /= v.value[1];
            value[2] /= v.value[2];
            return *this;
        }

        Vector3<T>& operator+=(T v)
        {
            value[0] += v;
            value[1] += v;
            value[2] += v;
            return *this;
        }

        Vector3<T>& operator*=(T v)
        {
            value[0] *= v;
            value[1] *= v;
            value[2] *= v;
            return *this;
        }

        Vector3<T>& operator/=(T num)
        {
            value[0] /= num;
            value[1] /= num;
            value[2] /= num;
            return *this;
        }

        Vector3<T> cross(const Vector3<T>& v)
        {
            Vector3<T> temp;
            temp[0] = value[1]*v.value[2] - value[2]*v.value[1];
            temp[1] = value[2]*v.value[0] - value[0]*v.value[2];
            temp[2] = value[0]*v.value[1] - value[1]*v.value[0];
            return temp;
        }
        double Magnitude()
        {
            return sqrt(pow(value[0],2)+pow(value[1],2)+pow(value[2],2));
        }

        Vector3<T> Normalize()
        {
            double mag = Magnitude();
            Vector3<T> temp(static_cast<T>(value[0]/mag),
                            static_cast<T>(value[1]/mag),
                            static_cast<T>(value[2]/mag));
            return temp;
        }

        static Vector3<T> XUnitVector()
        {
            return Vector3<T>(static_cast<T>(1.0),
                              static_cast<T>(0.0),
                              static_cast<T>(0.0));
        }

        static Vector3<T> YUnitVector()
        {
            return Vector3<T>(static_cast<T>(0.0),
                              static_cast<T>(1.0),
                              static_cast<T>(0.0));
        }

        static Vector3<T> ZUnitVector()
        {
            return Vector3<T>(static_cast<T>(0.0),
                              static_cast<T>(0.0),
                              static_cast<T>(1.0));
        }
    protected:
        T value[3];
    };

    template<typename T>
    double distance(const Vector3<T>& one, const Vector3<T>& two)
    {
        return sqrt((one[0]-two[0])*(one[0]-two[0])+
                    (one[1]-two[1])*(one[1]-two[1])+
                    (one[2]-two[2])*(one[2]-two[2]));
    }

    template<typename T>
    Vector3<T> operator-(const Vector3<T>& v)
    {
        Vector3<T> temp(-v[0],-v[1],-v[2]);
        return temp;
    }

    template<typename T>
    Vector3<T> operator+(const Vector3<T>& v1, const Vector3<T>& v2)
    {
        Vector3<T> r = v1;
        r += v2;
        return r;
    }

    template<typename T>
    Vector3<T> operator-(const Vector3<T>& v1, const Vector3<T>& v2)
    {
        Vector3<T> r = v1;
        r += -v2;
        return r;
    }

    template<typename T>
    Vector3<T> operator/(const Vector3<T>& v1, T num)
    {
        Vector3<T> r = v1;
        r /= num;
        return r;
    }

    template<typename T>
    Vector3<T> operator*(const Vector3<T>& v1, T num)
    {
        Vector3<T> r = v1;
        r *= num;
        return r;
    }

    template<typename T>
    Vector3<T> operator*(T num, const Vector3<T>& v1)
    {
        return v1*num;
    }

    const Vector3<float> zeroVector3f(0.0f, 0.0f, 0.0f);

    template<typename T>
    bool operator==(const Vector3<T>& one, const Vector3<T>& two)
    {
        return (one[0]==two[0] && one[1]==two[1] && one[2]==two[2]);
    }

}

#endif
