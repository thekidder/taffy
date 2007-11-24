//
// C++ Implementation: sore_math
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "sore_math.h"

namespace SORE_Math
{
	float Distance3D(float x1, float y1, float z1, float x2, float y2, float z2)
	{
		return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
	}
	
	float Distance2D(float x1, float y1, float x2, float y2)
	{
		return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
	}
}
