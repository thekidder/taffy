#include "app_log.h"
#include "kiss_spectrum.h"

#include <cassert>

float WindowFunc(float sample);

KISS_Spectrum::KISS_Spectrum(size_t fft_size, int sample_rate) 
    : FFTSpectrum(fft_size, sample_rate), timedata(fft_size), freqdata_left(fft_size), freqdata_right(fft_size),
    left_temp(NumBuckets()), right_temp(NumBuckets())
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
    for(int i = 0; i < samples - 1; ++i)
    {
        int k = i + 1; // first sample is average over all frequencies
        kiss_fft_scalar mag_left  = freqdata_left[k].r * freqdata_left[k].r + freqdata_left[k].i * freqdata_left[k].i;
        left_temp[i] = 10.0 * log10(static_cast<double>(mag_left) / SampleRate());

        kiss_fft_scalar mag_right  = freqdata_right[k].r * freqdata_right[k].r + freqdata_right[k].i * freqdata_right[k].i;
        right_temp[i] = 10.0 * log10(static_cast<double>(mag_right) / SampleRate());
    }
    SpectrumSnapshot new_snapshot(left_temp, right_temp);
    SetSnapshot(new_snapshot);
}

float WindowFunc(float sample)
{
    return sample;
}