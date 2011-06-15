#include "fmod_spectrum.h"

#include <fmod.hpp>

FMOD_Spectrum::FMOD_Spectrum(size_t fft_size, int sample_rate, FMOD::System* system_) 
    : FFTSpectrum(fft_size, sample_rate), system(system_), window_type(FMOD_DSP_FFT_WINDOW_HANNING),
    left_src(NumBuckets()), right_src(NumBuckets()), left_temp(NumBuckets()), right_temp(NumBuckets())
{
}

void FMOD_Spectrum::SetFMODSystem(FMOD::System* system_)
{
    system = system_;
}

void FMOD_Spectrum::SetWindowType(FMOD_DSP_FFT_WINDOW window)
{
    window_type = window;
}

void FMOD_Spectrum::Update()
{
    if(!system)
        return;

    if(system->getSpectrum(&left_src[0],  NumBuckets(), 0, window_type) != FMOD_OK)
        throw std::runtime_error("Could not get spectrum for left channel");
    if(system->getSpectrum(&right_src[0], NumBuckets(), 1, window_type) != FMOD_OK)
        throw std::runtime_error("Could not get spectrum for right channel");

    // convert to decibels
    for(size_t i = 0; i < NumBuckets(); ++i)
    {
        left_temp[i]  = 20.0 * log10(static_cast<double>(left_src[i]));
        right_temp[i] = 20.0 * log10(static_cast<double>(right_src[i]));
    }

    SpectrumSnapshot new_snapshot(left_temp, right_temp);
    SetSnapshot(new_snapshot);
}