#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "spectrum_snapshot.h"

class Spectrum
{
public:
    // number of buckets is fft_size / 2
    Spectrum(size_t num_buckets_);

    size_t NumBuckets() const { return num_buckets; }
    virtual double Value(size_t bucket, Audio_channel channel) const = 0; // return the value of the i'th bucket
    virtual const SpectrumSnapshot& Snapshot() const = 0; // retrieve a snapshot of the spectrum in this moment of time;

    virtual std::pair<double, double> HzRange(size_t bucket) const = 0;
    std::pair<double, double> TotalHz() const; 
private:
    size_t num_buckets;
};

#endif