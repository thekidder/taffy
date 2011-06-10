#ifndef GEOMETRIC_SPECTRUM_H
#define GEOMETRIC_SPECTRUM_H

#include "fft_spectrum.h"
#include "averaging_spectrum.h"

class GeometricSpectrum : public AveragingSpectrum
{
public:
    GeometricSpectrum(Spectrum& source_, size_t num_buckets_); 
};

#endif