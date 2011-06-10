#ifndef KISS_SPECTRUM_H
#define KISS_SPECTRUM_H

#include "fft_spectrum.h"

#undef FIXED_POINT
#include <kiss_fftr.h>

class KISS_Spectrum : public FFTSpectrum
{
public:
    KISS_Spectrum(size_t fft_size, int sample_rate);

    void AddSamples(float* buffer, unsigned int length, int channels);
private:
    kiss_fftr_cfg kiss_cfg;

    std::vector<kiss_fft_scalar> timedata;
    std::vector<kiss_fft_cpx> freqdata_left;
    std::vector<kiss_fft_cpx> freqdata_right;
};

#endif