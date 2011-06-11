#include "geometric_spectrum.h"

GeometricSpectrum::GeometricSpectrum(Spectrum& source_, size_t num_buckets_) 
    :  AveragingSpectrum(source_, num_buckets_)
{
    double max_hz = Source().TotalHz().second;
    double src_bucket_hz_size = max_hz / Source().NumBuckets();

    //constants for the ends of the first/last bands
    const double max_sample_rate = max_hz;
    const double min_sample_rate = src_bucket_hz_size;

    double multiplier = exp( log(max_sample_rate / min_sample_rate) / (NumBuckets() - 1));


    double min_rate = 0.0;
    size_t min_bucket = 0;
    for(size_t i = 0; i < NumBuckets(); ++i)
    {
        double max_rate;
        if(min_rate > 0)
            max_rate = min_rate *= multiplier;
        else
            max_rate = min_sample_rate;

        size_t max_bucket = min_bucket;

        while((max_bucket + 1) * src_bucket_hz_size < max_rate)
        {
            ++max_bucket;
        }

        bucket_src_ranges[i] = std::make_pair(min_bucket, max_bucket);

        min_bucket = max_bucket + 1;
        min_rate = max_rate;
    }
}