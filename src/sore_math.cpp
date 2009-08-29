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
	
	float Distance2DS(float x1, float y1, float x2, float y2)
	{
		return ((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
	}

	double Distance2DS(double x1, double y1, double x2, double y2)
	{
		return ((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
	}
	
	bool DoubleEquals(double value, double compare, double epsilon)
	{
		return fabs(value-compare)<epsilon;
	}
}
