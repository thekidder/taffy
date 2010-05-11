/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

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
