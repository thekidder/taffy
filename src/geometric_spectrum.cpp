#include "geometric_spectrum.h"

GeometricSpectrum::GeometricSpectrum(Spectrum& source_, size_t num_buckets_) 
    :  AveragingSpectrum(source_, num_buckets_)
{
    //constants for the ends of the first/last bands
    const double max_sample_rate = 20000.0;
    const double min_sample_rate = 20.0;

    double multiplier = exp( log(max_sample_rate / min_sample_rate) / (NumBuckets() - 1));

    double max_hz = Source().HzRange(Source().NumBuckets() - 1).second;
    double src_bucket_hz_size = max_hz / Source().NumBuckets();

    double min_rate = 0.0;
    size_t min_bucket = 1;
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