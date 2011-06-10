#ifndef GEOMETRIC_SPECTRUM_H
#define GEOMETRIC_SPECTRUM_H

#include "fft_spectrum.h"
#include "compressed_spectrum.h"

class GeometricSpectrum : public CompressedSpectrum
{
public:
    // first bucket is total energy
    GeometricSpectrum(FFTSpectrum& source_, size_t num_buckets_);

    virtual float Value(size_t bucket, Audio_channel channel) const;
    virtual std::pair<float, float> HzRange(size_t bucket);
private:
    std::vector<std::pair<size_t, size_t> > bucket_src_ranges;
};

#endif