#include "utility.h"

#include <algorithm>

float MapToRange(float value, Float_range_t original, Float_range_t newRange)
{
    // map to a percentage (0,1)
    float mag = (value - original.first) / (original.second - original.first);
    // map percentage to new range
    float transformed = newRange.first + (newRange.second - newRange.first) * mag;

    // clamp
    return std::min(newRange.second, std::max(transformed, newRange.first));
}

double MapToRange(double value, Double_range_t original, Double_range_t newRange)
{
    // map to a percentage (0,1)
    double mag = (value - original.first) / (original.second - original.first);
    // map percentage to new range
    double transformed = newRange.first + (newRange.second - newRange.first) * mag;

    // clamp
    return std::min(newRange.second, std::max(transformed, newRange.first));
}