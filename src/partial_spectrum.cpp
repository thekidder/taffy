#include "partial_spectrum.h"

PartialSpectrum::PartialSpectrum(Spectrum& source, size_t min_src_bucket, size_t max_src_bucket)
    : AveragingSpectrum(source, max_src_bucket - min_src_bucket)
{
    for(size_t i = min_src_bucket; i < max_src_bucket; ++i)
    {
        bucket_src_ranges[i - min_src_bucket] = std::make_pair(i, i);
    }
}