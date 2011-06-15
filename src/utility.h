#ifndef UTILITY_H
#define UTILITY_H

#include <utility>

typedef std::pair<float,float> Float_range_t;
typedef std::pair<double,double> Double_range_t;

const Float_range_t k_std_range_f(0.0f, 1.0f);
const Double_range_t k_std_range_d(0.0, 1.0);

float MapToRange(float value, Float_range_t original, Float_range_t newRange);
double MapToRange(double value, Double_range_t original, Double_range_t newRange);

#endif