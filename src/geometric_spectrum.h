#ifndef GEOMETRIC_SPECTRUM_H
#define GEOMETRIC_SPECTRUM_H

#include "compressed_spectrum.h"

class GeometricSpectrum : public CompressedSpectrum
{
public:
    // first bucket is total energy
    GeometricSpectrum(Spectrum& source_, size_t num_buckets_);

    virtual float Left (size_t bucket) const;
    virtual float Right(size_t bucket) const;
    virtual float Mix  (size_t bucket) const;

    virtual std::pair<float, float> HzRange(size_t bucket);
private:
    std::vector<std::pair<size_t, size_t> > bucket_src_ranges;
};

#endif