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
			
			NOISE_FUNC2D noiseGenerator;
			int seed;
			int numOctaves;
			double persistance;
	};
}
