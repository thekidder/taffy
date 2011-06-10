#include "geometric_spectrum.h"

GeometricSpectrum::GeometricSpectrum(FFTSpectrum& source_, size_t num_buckets_) 
    : CompressedSpectrum(source_, num_buckets_), bucket_src_ranges(NumBuckets())
{
    //constants for the ends of the first/last bands
    const double max_sample_rate = 20000.0;
    const double min_sample_rate = 20.0;

    double multiplier = exp( log(max_sample_rate / min_sample_rate) / (NumBuckets() - 1));

    double src_bucket_hz_size = Source().SampleRate() / 2.0 / Source().NumBuckets();

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

float GeometricSpectrum::Value(size_t bucket, Audio_channel channel) const
{
    size_t min_bucket = bucket_src_ranges[bucket].first;
    size_t max_bucket = bucket_src_ranges[bucket].first;

    float avg = 0.0f;
    for(size_t i = min_bucket; i <= max_bucket; ++i)
    {
        avg += Source().Value(i, channel);
    }
    avg /= (max_bucket - min_bucket + 1);

    return avg;
}

std::pair<float, float> GeometricSpectrum::HzRange(size_t bucket)
{
    float hz_per_src_bucket = Source().SampleRate() / 2.0f / Source().NumBuckets();

    size_t min_bucket = bucket_src_ranges[bucket].first;
    size_t max_bucket = bucket_src_ranges[bucket].first;

    return std::make_pair(min_bucket * hz_per_src_bucket, (max_bucket + 1) * hz_per_src_bucket);
}