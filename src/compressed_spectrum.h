#ifndef COMPRESSED_SPECTRUM_H
#define COMPRESSED_SPECTRUM_H

#include "fft_spectrum.h"
#include "spectrum.h"

class CompressedSpectrum : public Spectrum
{
public:
    CompressedSpectrum(FFTSpectrum& source_, size_t num_buckets_);

    virtual std::pair<float, float> HzRange(size_t bucket) = 0;
protected:
    const FFTSpectrum& Source() const;
private:
    const FFTSpectrum& source;
};

#endif