#ifndef UTILITY_H
#define UTILITY_H

#include <utility>

typedef std::pair<float,float> Float_range_t;

const Float_range_t k_std_range(0.0f, 1.0f);

float MapToRange(float value, Float_range_t original, Float_range_t newRange);

#endif