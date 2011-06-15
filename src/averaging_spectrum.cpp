#include "averaging_spectrum.h"

 AveragingSpectrum::AveragingSpectrum(Spectrum& source_, size_t num_buckets_) 
    : Spectrum(num_buckets_), bucket_src_ranges(NumBuckets()),
    source(source_), snapshot_cache(NumBuckets())
{
}

double AveragingSpectrum::Value(size_t bucket, Audio_channel channel) const
{
    size_t min_bucket = bucket_src_ranges[bucket].first;
    size_t max_bucket = bucket_src_ranges[bucket].first;

    double avg = 0.0f;
    for(size_t i = min_bucket; i <= max_bucket; ++i)
    {
        avg += Source().Value(i, channel);
    }
    avg /= (max_bucket - min_bucket + 1);

    return avg;
}

const SpectrumSnapshot& AveragingSpectrum::Snapshot() const
{
    std::vector<double> left(NumBuckets());
    std::vector<double> right(NumBuckets());
    for(size_t i = 0; i < NumBuckets(); ++i)
    {
        left[i] = Value(i, CHANNEL_LEFT);
        right[i] = Value(i, CHANNEL_RIGHT);
    }

    snapshot_cache = SpectrumSnapshot(left, right);

    return snapshot_cache;
}

std::pair<double, double> AveragingSpectrum::HzRange(size_t bucket) const
{
    size_t min_bucket = bucket_src_ranges[bucket].first;
    size_t max_bucket = bucket_src_ranges[bucket].second;

    return std::make_pair(Source().HzRange(min_bucket).first, Source().HzRange(max_bucket).second);
}

const Spectrum& AveragingSpectrum::Source() const
{
    return source;
}