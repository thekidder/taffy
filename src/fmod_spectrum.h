#ifndef FMOD_SPECTRUM_H
#define FMOD_SPECTRUM_H

#include "spectrum.h"

#include <fmod.hpp>

class FMOD_Spectrum : public Spectrum
{
public:
    FMOD_Spectrum(size_t fft_size, int sample_rate, FMOD::System* system_);

    void SetWindowType(FMOD_DSP_FFT_WINDOW window);
    void Update();
private:
    FMOD::System* system;
    FMOD_DSP_FFT_WINDOW window_type;
};

#endif