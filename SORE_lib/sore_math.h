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

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

#include <cmath>

#include "math/sore_vector2.h"
#include "math/sore_vector3.h"
#include "math/sore_matrix4x4.h"

namespace SORE_Math
{
	const double M_PI_180 = M_PI/180.0;	
	
	typedef Vector3<double> Vector3d;
	typedef Vector3<float>  Vector3f;
	typedef Vector3<int>    Vector3i;
	typedef Vector2<double> Vector2d;
	typedef Vector2<float>  Vector2f;
	typedef Vector2<int>    Vector2i;
	
	float  Distance3D(float x1, float y1, float z1, float x2, float y2, float z2);
	float  Distance2D(float x1, float y1, float x2, float y2);
	float  Distance2DS(float x1, float y1, float x2, float y2); //distance squared
	double Distance2DS(double x1, double y1, double x2, double y2);
	
	bool   DoubleEquals(double value, double compare, double epsilon);
}

#endif /*__SORE_MATH_H__*/
