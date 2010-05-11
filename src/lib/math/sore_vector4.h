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

#ifndef _SORE_VECTOR4_H_
#define _SORE_VECTOR4_H_

#include <cassert>

#include <sore_vector3.h>

namespace SORE_Math
{
    template<typename T>
    class SORE_EXPORT Vector4
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

        const T* GetValue() const
        {
            return value;
        }

        const T& operator[] (int index) const
        {
            if(index<0 || index>3)
            {
                assert("Index out of range!");
                return value[0];
            }
            return value[index];
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

        T dot(const Vector4<T>& v)
        {
            T temp = v[0]*value[0] + v[1]*value[1] + v[2]*value[2] + v[3]*value[3];
            return temp;
        }

        Vector4<T>& operator+=(const Vector4<T> v)
        {
            value[0] += v.value[0];
            value[1] += v.value[1];
            value[2] += v.value[2];
            value[3] += v.value[3];
            return *this;
        }

        Vector4<T>& operator*=(const Vector4<T> v)
        {
            value[0] *= v.value[0];
            value[1] *= v.value[1];
            value[2] *= v.value[2];
            value[3] *= v.value[3];
            return *this;
        }

        Vector4<T>& operator/=(const Vector4<T>& v)
        {
            value[0] /= v.value[0];
            value[1] /= v.value[1];
            value[2] /= v.value[2];
            value[3] /= v.value[3];
            return *this;
        }

        Vector4<T>& operator+=(T v)
        {
            value[0] += v;
            value[1] += v;
            value[2] += v;
            value[3] += v;
            return *this;
        }

        Vector4<T>& operator*=(T v)
        {
            value[0] *= v;
            value[1] *= v;
            value[2] *= v;
            value[3] *= v;
            return *this;
        }

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
            return sqrt(
                pow(value[0],2)+
                pow(value[1],2)+
                pow(value[2],2)+
                pow(value[3],2));
        }

        Vector4 Normalize()
        {
            double mag = Magnitude();
            Vector4<T> temp(static_cast<T>(value[0]/mag),
                            static_cast<T>(value[1]/mag),
                            static_cast<T>(value[2]/mag),
                            static_cast<T>(value[3]/mag));
            return temp;
        }

    protected:
        T value[4];
    };

    template<typename T>
    double distance(const Vector4<T>& one, const Vector4<T>& two)
    {
        return sqrt((one[0]-two[0])*(one[0]-two[0])+
                    (one[1]-two[1])*(one[1]-two[1])+
                    (one[2]-two[2])*(one[2]-two[2])+
                    (one[3]-two[3])*(one[3]-two[3]));
    }

    template<typename T>
    Vector4<T> operator-(const Vector4<T>& v)
    {
        Vector4<T> temp(-v[0],-v[1],-v[2], -v[3]);
        return temp;
    }

    template<typename T>
    Vector4<T> operator+(const Vector4<T>& v1, const Vector4<T>& v2)
    {
        Vector4<T> r = v1;
        r += v2;
        return r;
    }

    template<typename T>
    Vector4<T> operator-(const Vector4<T>& v1, const Vector4<T>& v2)
    {
        Vector4<T> r = v1;
        r += -v2;
        return r;
    }

    template<typename T>
    Vector4<T> operator/(const Vector4<T>& v1, T& num)
    {
        Vector4<T> r = v1;
        r /= num;
        return r;
    }

    template<typename T>
    Vector4<T> operator*(const Vector4<T>& v1, T num)
    {
        Vector4<T> r = v1;
        r *= num;
        return r;
    }

    template<typename T>
    Vector4<T> operator*(T num, const Vector4<T>& v1)
    {
        return v1*num;
    }

    template<typename T>
    bool operator==(const Vector4<T>& one, const Vector4<T>& two)
    {
        return one[0] == two[0] && one[1] == two[1]
            && one[2] == two[2] && one[3] == two[3];
    }
}

#endif
