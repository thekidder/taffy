#include "fmod_spectrum.h"

#include <fmod.hpp>

FMOD_Spectrum::FMOD_Spectrum(size_t fft_size, int sample_rate, FMOD::System* system_) 
    : Spectrum(fft_size, sample_rate), system(system_), window_type(FMOD_DSP_FFT_WINDOW_HANNING)
{
}

void FMOD_Spectrum::SetWindowType(FMOD_DSP_FFT_WINDOW window)
{
    window_type = window;
}

void FMOD_Spectrum::Update()
{
    if(system->getSpectrum(&SetLeft(0),  NumBuckets(), 0, window_type) != FMOD_OK)
        throw std::runtime_error("Could not get spectrum for left channel");
    if(system->getSpectrum(&SetRight(0), NumBuckets(), 1, window_type) != FMOD_OK)
        throw std::runtime_error("Could not get spectrum for right channel");

    // convert to decibels
    for(size_t i = 0; i < NumBuckets(); ++i)
    {
        SetLeft(i)  = 20.0f * log10(Left(i));
        SetRight(i) = 20.0f * log10(Right(i));
    }
}