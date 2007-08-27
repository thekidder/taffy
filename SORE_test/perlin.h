//
// C++ Interface: perlin
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef  __PERLIN_H__
#define  __PERLIN_H__

#include "noise.h"

namespace Noise
{
	
	
	class PerlinNoise
	{
		public:
			PerlinNoise(NOISE_FUNC2D noiseGen, int seed, int num, double p);
			double GetValue(double x, double y);
		protected:
			double SmoothedNoise(double x, double y);
			double InterpolatedNoise(double x, double y);
			double CosineInterpolate(double a, double b, double x);
			
			NOISE_FUNC2D noiseGenerator;
			int seed;
			int numOctaves;
			double persistance;
	};
}

#endif /*__PERLIN_H__*/
