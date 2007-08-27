//
// C++ Implementation: noise
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "noise.h"
#include <cstdlib>

double Noise::RandomNoise1D(double value, int seed)
{
	srand(seed);
	return double(rand()) / RAND_MAX;
}

double Noise::Noise12D(int x, int y, int seed)
{
	int n = x + y * 57;
	n = (n<<6) ^ n;
	return ( 1.0 - double( int(n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);  
}
