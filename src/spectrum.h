#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "spectrum_snapshot.h"

class Spectrum
{
public:
    // number of buckets is fft_size / 2
    Spectrum(size_t num_buckets_);

    size_t NumBuckets() const { return num_buckets; }
    virtual float Value(size_t bucket, Audio_channel channel) const = 0;
private:
    size_t num_buckets;
};

#endif