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

#ifndef _SORE_VECTOR2_H_
#define _SORE_VECTOR2_H_

#define _USE_MATH_DEFINES
#include <cassert>
#include <cmath>

#include <sore_dll.h>

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

        const T* GetValue() const
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

        Vector2<T>& operator+=(const Vector2<T>& v)
        {
            value[0] += v.value[0];
            value[1] += v.value[1];
            return *this;
        }
        Vector2<T>& operator-=(const Vector2<T>& v)
        {
            value[0] -= v.value[0];
            value[1] -= v.value[1];
            return *this;
        }

        Vector2<T>& operator*=(const Vector2<T>& v)
        {
            value[0] *= v.value[0];
            value[1] *= v.value[1];
            return *this;
        }

        Vector2<T>& operator/=(const Vector2<T>& v)
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
    double distance(const Vector2<T>& one, const Vector2<T>& two)
    {
        return sqrt((one[0]-two[0])*(one[0]-two[0])+(one[1]-two[1])*(one[1]-two[1]));
    }

    template<typename T>
    Vector2<T> operator-(const Vector2<T>& v)
    {
        Vector2<T> temp(-v[0],-v[1]);
        return temp;
    }

    template<typename T>
    Vector2<T> operator+(const Vector2<T>& v1, const Vector2<T>& v2)
    {
        Vector2<T> r = v1;
        r += v2;
        return r;
    }

    template<typename T>
    Vector2<T> operator-(const Vector2<T>& v1, const Vector2<T>& v2)
    {
        Vector2<T> r = v1;
        r += -v2;
        return r;
    }

    template<typename T>
    Vector2<T> operator/(const Vector2<T>& v1, T& num)
    {
        Vector2<T> r = v1;
        r /= num;
        return r;
    }

    template<typename T>
    Vector2<T> operator*(const Vector2<T>& v1, T num)
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
    bool operator>(const Vector2<T>& one, const Vector2<T>& two)
    {
        return one.Magnitude() > two.Magnitude();
    }

    template<typename T>
    bool operator==(const Vector2<T>& one, const Vector2<T>& two)
    {
        return one[0] == two[0] && one[1] == two[1];
    }
}

#endif
