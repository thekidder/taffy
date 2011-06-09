#include "app_log.h"
#include "kiss_spectrum.h"

#include <cassert>

float WindowFunc(float sample);

KISS_Spectrum::KISS_Spectrum(size_t fft_size, int sample_rate) 
    : FFTSpectrum(fft_size, sample_rate), timedata(fft_size), freqdata_left(fft_size), freqdata_right(fft_size)
{
    kiss_cfg = kiss_fftr_alloc(fft_size, 0, 0, 0);
}

void KISS_Spectrum::AddSamples(float* buffer, unsigned int length, int channels)
{
    assert(channels == 2);
    assert(length == FFTSize() * channels);

    //calculate left fft
    for(size_t i = 0; i < FFTSize(); ++i)
    {
        timedata[i] = WindowFunc(buffer[i*2]);
    }

    kiss_fftr(kiss_cfg, &timedata[0], &freqdata_left[0]);

    //calculate right fft
    for(size_t i = 0; i < FFTSize(); ++i)
    {
        timedata[i] = WindowFunc(buffer[i*2 + 1]);
    }

    kiss_fftr(kiss_cfg, &timedata[0], &freqdata_right[0]);

    const int samples = NumBuckets();
    std::vector<float> left(samples);
    std::vector<float> right(samples);
    for(int i = 0; i < samples - 1; ++i)
    {
        int k = i + 1; // first sample is average over all frequencies
        kiss_fft_scalar mag_left  = freqdata_left[k].r * freqdata_left[k].r + freqdata_left[k].i * freqdata_left[k].i;
        left[i] = 10.0f * log10(mag_left / SampleRate());

        kiss_fft_scalar mag_right  = freqdata_right[k].r * freqdata_right[k].r + freqdata_right[k].i * freqdata_right[k].i;
        right[i] = 10.0f * log10(mag_right / SampleRate());
    }
    SpectrumSnapshot new_snapshot(left, right);
    SetSnapshot(new_snapshot);
}

float WindowFunc(float sample)
{
    return sample;
}