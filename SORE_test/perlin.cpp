//
// C++ Implementation: perlin
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "perlin.h"
#include <cmath>

Noise::PerlinNoise::PerlinNoise(NOISE_FUNC2D noiseGen, int s, int num, double p)
{
	seed = s;
	noiseGenerator = noiseGen;
	persistance = p;
	numOctaves = num;
}

double Noise::PerlinNoise::GetValue(double x, double y)
{
	double total = 0.0;
	for(int i=0;i<numOctaves;i++)
	{
		double frequency = pow(2.0,i);
		double amplitude = pow(persistance,i);
		total += SmoothedNoise(x*frequency,y*frequency)*amplitude;
	}
	return total;
}

double Noise::PerlinNoise::SmoothedNoise(double x, double y)
{
	double corners = (noiseGenerator(x-1,y-1,seed)+noiseGenerator(x+1,y-1,seed)+noiseGenerator(x+1,y+1,seed)+noiseGenerator(x-1,y+1,seed))/16.0;
	double sides = (noiseGenerator(x+1,y,seed)+noiseGenerator(x-1,y,seed)+noiseGenerator(x,y+1,seed)+noiseGenerator(x,y-1,seed))/8.0;
	double center = noiseGenerator(x,y,seed)/4.0;
	return sides + center + corners;
}
