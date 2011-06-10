#include "compressed_spectrum.h"

CompressedSpectrum::CompressedSpectrum(FFTSpectrum& source_, size_t num_buckets_) 
    : Spectrum(num_buckets_), source(source_), snapshot_cache(NumBuckets())
{
}

const SpectrumSnapshot& CompressedSpectrum::Snapshot() const
{
    std::vector<float> left(NumBuckets());
    std::vector<float> right(NumBuckets());
    for(size_t i = 0; i < NumBuckets(); ++i)
    {
        left[i] = Value(i, CHANNEL_LEFT);
        right[i] = Value(i, CHANNEL_RIGHT);
    }

    snapshot_cache = SpectrumSnapshot(left, right);

    return snapshot_cache;
}

const FFTSpectrum& CompressedSpectrum::Source() const
{
    return source;
}