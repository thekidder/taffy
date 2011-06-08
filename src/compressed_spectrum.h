#ifndef COMPRESSED_SPECTRUM_H
#define COMPRESSED_SPECTRUM_H

#include "spectrum.h"

class CompressedSpectrum
{
public:
    CompressedSpectrum(Spectrum& source_, size_t num_buckets_);

    size_t NumBuckets() const { return num_buckets; }

    virtual float Left (size_t bucket) const = 0;
    virtual float Right(size_t bucket) const = 0;
    virtual float Mix  (size_t bucket) const = 0;

    virtual std::pair<float, float> HzRange(size_t bucket) = 0;
protected:
    const Spectrum& Source() const;
private:
    const Spectrum& source;
    size_t num_buckets;
};

#endif