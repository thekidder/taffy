#ifndef PARTIAL_SPECTRUM_H
#define PARTIAL_SPECTRUM_H

#include "averaging_spectrum.h"

// includes all the bins of the source spectrum between [min_src_bucket, max_src_bucket)
class PartialSpectrum : public AveragingSpectrum
{
public:
    PartialSpectrum(Spectrum& source, size_t min_src_bucket, size_t max_src_bucket);
};

#endif