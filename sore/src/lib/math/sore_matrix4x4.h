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

#ifndef SORE_MATRIX4X4_H
#define SORE_MATRIX4X4_H

#include <sore_math_defines.h>
#include <sore_vector2.h>
#include <sore_vector3.h>
#include <sore_vector4.h>


namespace SORE_Math
{
    const unsigned int AXIS_X = 0;
    const unsigned int AXIS_Y = 1;
    const unsigned int AXIS_Z = 2;

    template<typename T>
    class  Matrix4
    {
        public:
            Matrix4()
            {
                SetIdentity();
            }

            Matrix4(T* v)
            {
                for(unsigned int i=0;i<16;++i)
                    value[i] = v[i];
            }

            void Rotate(T rads, unsigned int axis)
            {
                Matrix4<T> rotation = Matrix4<T>::GetRotation(rads, axis);
                *this *= rotation;
            }

            void Rotate(T rads, SORE_Math::Vector3<T> axis)
            {
                Matrix4<T> rotation = Matrix4<T>::GetRotation(rads, axis);
                *this *= rotation;
            }

            void Translate(T x, T y, T z)
            {
                Matrix4<T> translate = Matrix4<T>::GetTranslation(x, y, z);
                *this *= translate;
            }

            void SetPosition(T x, T y, T z)
            {
                value[12] = x;
                value[13] = y;
                value[14] = z;
            }

            void Scale(T x, T y, T z)
            {
                Matrix4<T> scale = Matrix4<T>::GetScale(x, y, z);
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

            Matrix4<T>& operator*=(const Matrix4<T>& m)
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

            Matrix4<T> Transpose()
            {
                T newValues[16];
                for(unsigned int c=0;c<4;++c)
                {
                    for(unsigned int r=0;r<4;++r)
                    {
                        newValues[c + r*4] = value[r + c*4];
                    }
                }
                return Matrix4<T>(newValues);
            }

            bool Normalized()
            {
                float curr = 0.0;
                for(unsigned int c=0;c<4;++c)
                {
                    for(unsigned int r=0;r<4;++r)
                    {
                        curr += value[c + r*4]*value[c + r*4];
                    }
                    if(fabs(curr-1.0) > 0.001)
                        return false;
                    curr = 0.0;
                }

                for(unsigned int r=0;r<4;++r)
                {
                    for(unsigned int c=0;c<4;++c)
                    {
                        curr += value[c + r*4]*value[c + r*4];
                    }
                    if(fabs(curr-1.0) > 0.001)
                        return false;
                    curr = 0.0;
                }
                return true;
            }

            static Matrix4<T> GetRotation(T rads, unsigned int axis)
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

            static Matrix4<T> GetRotation(T rads, SORE_Math::Vector3<T> axis)
            {
                axis = axis.Normalize();
                T c = cos(rads);
                T s = sin(rads);
                T raw[16];
                raw[ 0] = axis[0]*axis[0]*(1-c)+c;
                raw[ 1] = axis[1]*axis[0]*(1-c)+axis[2]*s;
                raw[ 2] = axis[0]*axis[2]*(1-c)-axis[1]*s;
                raw[ 3] = 0.0;
                raw[ 4] = axis[0]*axis[1]*(1-c)-axis[2]*s;
                raw[ 5] = axis[1]*axis[1]*(1-c)+c;
                raw[ 6] = axis[1]*axis[2]*(1-c)+axis[0]*s;
                raw[ 7] = 0.0;
                raw[ 8] = axis[0]*axis[2]*(1-c)+axis[1]*s;
                raw[ 9] = axis[1]*axis[2]*(1-c)-axis[0]*s;
                raw[10] = axis[2]*axis[2]*(1-c)+c;
                raw[11] = 0.0;
                raw[12] = 0.0;
                raw[13] = 0.0;
                raw[14] = 0.0;
                raw[15] = 1.0;
                Matrix4<T> temp(raw);
                return temp;
            }

            static Matrix4<T> GetTranslation(T x, T y, T z)
            {
                Matrix4<T> temp;
                temp.value[12] = x;
                temp.value[13] = y;
                temp.value[14] = z;
                return temp;
            }

            static Matrix4<T> GetTranslation(const Vector3<T>& t)
            {
                Matrix4<T> temp;
                temp.value[12] = t[0];
                temp.value[13] = t[1];
                temp.value[14] = t[2];
                return temp;
            }

            static Matrix4<T> GetScale(T x, T y, T z)
            {
                Matrix4<T> temp;
                temp.value[0 ] = x;
                temp.value[5 ] = y;
                temp.value[10] = z;
                return temp;
            }

            static Matrix4<T> GetScale(const Vector3<T>& s)
            {
                Matrix4<T> temp;
                temp.value[0 ] = s[0];
                temp.value[5 ] = s[1];
                temp.value[10] = s[2];
                return temp;
            }

            static Matrix4<T> GetLookat(
                const Vector3<T>& eye,
                const Vector3<T>& center,
                const Vector3<T>& up)
            {
                Vector3<T> forward = center - eye;
                forward = forward.Normalize();
                Vector3<T> up_ = up.Normalize();
                Vector3<T> side = forward.cross(up_);
                up_ = side.cross(forward);

                Matrix4<T> temp;
                temp.value[0 ] = side[0];
                temp.value[4 ] = side[1];
                temp.value[8 ] = side[2];
                temp.value[12] = T(0.0);

                temp.value[1 ] = up_[0];
                temp.value[5 ] = up_[1];
                temp.value[9 ] = up_[2];
                temp.value[13] = T(0.0);

                temp.value[2 ] = -forward[0];
                temp.value[6 ] = -forward[1];
                temp.value[10] = -forward[2];
                temp.value[14] = T(0.0);

                temp.value[3 ] = T(0.0);
                temp.value[7 ] = T(0.0);
                temp.value[11] = T(0.0);
                temp.value[15] = T(1.0);

                temp *= GetTranslation(-eye);

                return temp;
            }

            static Matrix4<T> GetPerspective(T fovy, T aspectRatio, T znear, T zfar)
            {
                T ymax = znear * tanf(fovy * T(kPi) / T(360.0));
                T xmax = ymax * aspectRatio;

                return GetFrustum(-xmax, xmax, -ymax, ymax, znear, zfar);
            }

            static Matrix4<T> GetOrtho(T left, T right, T top, T bottom, T znear, T zfar)
            {
                Matrix4<T> result;
                result.value[0 ] = T(2.0) / (right - left);
                result.value[5 ] = T(2.0) / (top - bottom);
                result.value[10] = T(-2.0) / (zfar - znear);
                result.value[12] = -(right + left) / (right - left);
                result.value[13] = -(top + bottom) / (top - bottom);
                result.value[14] = -(zfar + znear) / (zfar - znear);

                return result;
            }

            static Matrix4<T> GetFrustum(T left, T right, T bottom, T top, T znear, T zfar)
            {
                T dist = T(2.0) * znear;
                T width = right - left;
                T height = top - bottom;
                T depth = zfar - znear;

                Matrix4<T> temp;
                temp.value[0 ] = dist / width;
                temp.value[1 ] = T(0.0);
                temp.value[2 ] = T(0.0);
                temp.value[3 ] = T(0.0);

                temp.value[4 ] = T(0.0);
                temp.value[5 ] = dist / height;
                temp.value[6 ] = T(0.0);
                temp.value[7 ] = T(0.0);

                temp.value[8 ] = (right + left) / width;
                temp.value[9 ] = (top + bottom) / height;
                temp.value[10] = (-zfar - znear) / depth;
                temp.value[11] = T(-1.0);

                temp.value[12] = T(0.0);
                temp.value[13] = T(0.0);
                temp.value[14] = (-dist * zfar) / depth;
                temp.value[15] = T(0.0);

                return temp;
            }

            T* GetData()
            {
                return &value[0];
            }

            const T* GetData() const
            {
                return &value[0];
            }

        protected:
            T value[16];
    };

    template<typename T>
            Matrix4<T> operator*(Matrix4<T> m1, Matrix4<T> m2)
    {
        Matrix4<T> temp = m1;
        m1 *= m2;
        return m1;
    }

    template<typename T>
            Vector4<T> operator*(const Matrix4<T>& m1, const Vector4<T>& v1)
    {
        Vector4<T> temp;
        temp[0] = m1.GetData()[ 0]*v1[0] + m1.GetData()[ 4]*v1[1] + m1.GetData()[ 8]*v1[2] + m1.GetData()[12]*v1[3];
        temp[1] = m1.GetData()[ 1]*v1[0] + m1.GetData()[ 5]*v1[1] + m1.GetData()[ 9]*v1[2] + m1.GetData()[13]*v1[3];
        temp[2] = m1.GetData()[ 2]*v1[0] + m1.GetData()[ 6]*v1[1] + m1.GetData()[10]*v1[2] + m1.GetData()[14]*v1[3];
        temp[3] = m1.GetData()[ 3]*v1[0] + m1.GetData()[ 7]*v1[1] + m1.GetData()[11]*v1[2] + m1.GetData()[15]*v1[3];
        return temp;
    }

    template<typename T>
            Vector3<T> operator*(const Matrix4<T>& m, const Vector3<T>& v)
    {
        Vector4<T> temp(v[0], v[1], v[2], static_cast<T>(1));
        temp = m * temp;
        Vector3<T> result(temp[0], temp[1], temp[2]);
        return result;
    }


    template<typename T>
    Vector2<T> operator*(const Matrix4<T>& m, const Vector2<T>& v)
    {
        Vector4<T> temp(v[0], v[1], static_cast<T>(1), static_cast<T>(1));
        temp = m * temp;
        Vector2<T> result(temp[0], temp[1]);
        return result;
    }

    template<typename T>
    bool operator==(const Matrix4<T>& one, const Matrix4<T>& two)
    {
        return one.GetData()[ 0] == two.GetData()[ 0]
            && one.GetData()[ 1] == two.GetData()[ 1]
            && one.GetData()[ 2] == two.GetData()[ 2]
            && one.GetData()[ 3] == two.GetData()[ 3]
            && one.GetData()[ 4] == two.GetData()[ 4]
            && one.GetData()[ 5] == two.GetData()[ 5]
            && one.GetData()[ 6] == two.GetData()[ 6]
            && one.GetData()[ 7] == two.GetData()[ 7]
            && one.GetData()[ 8] == two.GetData()[ 8]
            && one.GetData()[ 9] == two.GetData()[ 9]
            && one.GetData()[10] == two.GetData()[10]
            && one.GetData()[11] == two.GetData()[11]
            && one.GetData()[12] == two.GetData()[12]
            && one.GetData()[13] == two.GetData()[13]
            && one.GetData()[14] == two.GetData()[14]
            && one.GetData()[15] == two.GetData()[15];
    }

    template<typename T>
    bool operator!=(const Matrix4<T>& one, const Matrix4<T>& two)
    {
        return !(one == two);
    }
}

#endif
