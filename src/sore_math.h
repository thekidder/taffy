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
// $Id$

#ifndef  SORE_MATH_H
#define  SORE_MATH_H

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

#include <cmath>

#include "math/sore_vector2.h"
#include "math/sore_vector3.h"
#include "math/sore_vector4.h"
#include "math/sore_matrix4x4.h"
#include "math/sore_quaternion.h"

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

#endif
