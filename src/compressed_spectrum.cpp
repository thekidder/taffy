#include "compressed_spectrum.h"

CompressedSpectrum::CompressedSpectrum(Spectrum& source_, size_t num_buckets_) 
    : source(source_), num_buckets(num_buckets_)
{
}

const Spectrum& CompressedSpectrum::Source() const
{
    return source;
}