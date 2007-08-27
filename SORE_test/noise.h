//
// C++ Interface: noise
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

namespace Noise
{
	typedef double(*NOISE_FUNC1D)(double, int); //the int is used for a possible seed
	typedef double(*NOISE_FUNC2D)(int, int, int); //the int is used for a possible seed
	
	double RandomNoise1D(double value, int seed);
	double RandomNoise2D(int x, int y, int seed);
	
	double Noise12D(int x, int y, int seed);
}
