#include "log_spectrograph.h"

#include <cassert>

const float kMaxHz = 20000.0f;

LogSpectrograph::LogSpectrograph(int sampleRate_, int fftSamples, size_t numBuckets)
    : Spectrograph(numBuckets), sampleRate(sampleRate_), kFFTSamples(fftSamples), 
	  timedata(kFFTSamples), freqdata(kFFTSamples/2 + 1)
{
    kiss_cfg = kiss_fftr_alloc(kFFTSamples, 0, 0, 0);
}

LogSpectrograph::~LogSpectrograph()
{
    kiss_fftr_free(kiss_cfg);
}

void LogSpectrograph::AddSamples(float* buffer, unsigned int length, int channels)
{
    assert(channels == 2);
    assert(length == kFFTSamples * channels);
    // APP_LOG(SORE_Logging::LVL_INFO, boost::format("Got %d samples (%d channels)") % length % channels);

    //calculate mono fft
    for(int i = 0; i < kFFTSamples; ++i)
    {
        timedata[i] = (buffer[i*2] + buffer[i*2]) / 2.0f;
    }

    kiss_fftr(kiss_cfg, &timedata[0], &freqdata[0]);

    Clear();
    // divide by two: only get resolution out of bottom half of fft
    const int samples = (kFFTSamples / 2);
    // how much of the FFT data to analyze: only care about bottom 20k Hz
    const double fftFraction = kMaxHz / sampleRate;
    const double samples_per_window = (samples * fftFraction) / NumBuckets();
    //APP_LOG(SORE_Logging::LVL_INFO, boost::format("Need to analyze %d samples of width %d Hz (sample rate = %d)") % samples % (sample_rate / samples) % sample_rate);
    for(int i = 0; i < (int)(samples * fftFraction) - 1; ++i)
    {
        int k = i+1; // first sample is average over all frequencies
        kiss_fft_scalar mag  = freqdata[k].r * freqdata[k].r + freqdata[k].i * freqdata[k].i;
        if(mag > 0.0f)
            Set(static_cast<int>(i / samples_per_window)) += 20.0f * log10(mag);
        //APP_LOG(SORE_Logging::LVL_INFO, boost::format("Got sample: (%f, %f) magnitude %f") % freqdata[k].r % freqdata[k].i % mag);
    }

    for(int i = 0; i < NumBuckets(); ++i)
    {
        // average each bucket
        Set(i) /= samples_per_window;
    }
}

std::pair<float, float> LogSpectrograph::BucketRange(size_t bucket) const
{
    // divide our range into buckets:
    float width = kMaxHz / NumBuckets();
    return std::make_pair(width * bucket, (width + 1) * bucket);
}