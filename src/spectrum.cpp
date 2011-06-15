#include "spectrum.h"

Spectrum::Spectrum(size_t num_buckets_) 
    : num_buckets(num_buckets_)
{
}

std::pair<double, double> Spectrum::TotalHz() const
{
    return std::make_pair(HzRange(0).first, HzRange(NumBuckets() - 1).second);
}