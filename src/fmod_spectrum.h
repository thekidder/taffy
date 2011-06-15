#ifndef FMOD_SPECTRUM_H
#define FMOD_SPECTRUM_H

#include "fft_spectrum.h"

#include <fmod.hpp>

class FMOD_Spectrum : public FFTSpectrum
{
public:
    FMOD_Spectrum(size_t fft_size, int sample_rate, FMOD::System* system_);

    void SetWindowType(FMOD_DSP_FFT_WINDOW window);
    void Update();
private:
    FMOD::System* system;
    FMOD_DSP_FFT_WINDOW window_type;

    // some temporary buffers for getting and transforming the data
    // keep them here to reduce allocations
    std::vector<float> left_src, right_src;
    std::vector<double> left_temp, right_temp;
};

#endif