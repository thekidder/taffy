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
		total += InterpolatedNoise(x*frequency,y*frequency)*amplitude;
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

double Noise::PerlinNoise::InterpolatedNoise(double x, double y)
{
	int intX = int(x);
	int intY = int(y);
	
	double fracX = x - intX;
	double fracY = y - intY;
	
	double v1,v2,v3,v4;
	v1 = SmoothedNoise(intX, intY);
	v2 = SmoothedNoise(intX+1, intY);
	v3 = SmoothedNoise(intX,intY+1);
	v4 = SmoothedNoise(intX+1,intY+1);
	
	double il1,il2;
	il1 = CosineInterpolate(v1,v2, fracX);
	il2 = CosineInterpolate(v3,v4, fracX);
	
	return CosineInterpolate(il1,il2,fracY);
}

double Noise::PerlinNoise::CosineInterpolate(double a, double b, double x)
{
	double ft = x * M_PI;
	double f = (1 - cos(ft)) * 0.5f;
	return  a*(1-f) + b*f;
}
