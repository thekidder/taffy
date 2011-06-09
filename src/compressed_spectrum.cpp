#include "compressed_spectrum.h"

CompressedSpectrum::CompressedSpectrum(FFTSpectrum& source_, size_t num_buckets_) 
    : Spectrum(num_buckets_), source(source_)
{
}

const FFTSpectrum& CompressedSpectrum::Source() const
{
    return source;
}