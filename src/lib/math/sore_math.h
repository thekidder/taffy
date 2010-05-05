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
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
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

#ifndef  SORE_MATH_H
#define  SORE_MATH_H

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

#include <cmath>

#include <sore_vector2.h>
#include <sore_vector3.h>
#include <sore_vector4.h>
#include <sore_matrix4x4.h>
#include <sore_quaternion.h>

namespace SORE_Math
{
	const double M_PI_180 = M_PI/180.0;	
	
	typedef Vector3<double> Vector3d;
	typedef Vector3<float>  Vector3f;
	typedef Vector3<int>    Vector3i;
	typedef Vector2<double> Vector2d;
	typedef Vector2<float>  Vector2f;
	typedef Vector2<int>    Vector2i;
	
	float  SORE_EXPORT Distance3D(float x1, float y1, float z1, float x2, float y2, float z2);
	float  SORE_EXPORT Distance2D(float x1, float y1, float x2, float y2);
	float  SORE_EXPORT Distance2DS(float x1, float y1, float x2, float y2); //distance squared
	double SORE_EXPORT Distance2DS(double x1, double y1, double x2, double y2);
	
	bool   SORE_EXPORT DoubleEquals(double value, double compare, double epsilon);
}

#endif
