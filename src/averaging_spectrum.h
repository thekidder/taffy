#ifndef COMPRESSED_SPECTRUM_H
#define COMPRESSED_SPECTRUM_H

#include "fft_spectrum.h"
#include "spectrum.h"

// takes a source spectrum and 
class AveragingSpectrum : public Spectrum
{
public:
     AveragingSpectrum(Spectrum& source_, size_t num_buckets_);

    virtual float Value(size_t bucket, Audio_channel channel) const;
    virtual const SpectrumSnapshot& Snapshot() const;

    virtual std::pair<float, float> HzRange(size_t bucket) const;
protected:
    const Spectrum& Source() const;

    std::vector<std::pair<size_t, size_t> > bucket_src_ranges;
private:
    const Spectrum& source;

    mutable SpectrumSnapshot snapshot_cache;
};

#endif