#include "fmod_spectrum.h"

#include <fmod.hpp>

FMOD_Spectrum::FMOD_Spectrum(size_t fft_size, int sample_rate, FMOD::System* system_) 
    : FFTSpectrum(fft_size, sample_rate), system(system_), window_type(FMOD_DSP_FFT_WINDOW_HANNING)
{
}

void FMOD_Spectrum::SetWindowType(FMOD_DSP_FFT_WINDOW window)
{
    window_type = window;
}

void FMOD_Spectrum::Update()
{
    std::vector<float> left(NumBuckets());
    std::vector<float> right(NumBuckets());
    if(system->getSpectrum(&left[0],  NumBuckets(), 0, window_type) != FMOD_OK)
        throw std::runtime_error("Could not get spectrum for left channel");
    if(system->getSpectrum(&right[0], NumBuckets(), 1, window_type) != FMOD_OK)
        throw std::runtime_error("Could not get spectrum for right channel");

    // convert to decibels
    for(size_t i = 0; i < NumBuckets(); ++i)
    {
        left[i]  = 20.0f * log10(left[i]);
        right[i] = 20.0f * log10(right[i]);
    }

    SpectrumSnapshot new_snapshot(left, right);
    SetSnapshot(new_snapshot);
}