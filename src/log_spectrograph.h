#ifndef LOG_SPECTROGRAPH_H
#define LOG_SPECTROGRAPH_H

#include "spectrograph.h"

#include <kiss_fftr.h>

class LogSpectrograph : public Spectrograph
{
public:
    LogSpectrograph(int sampleRate_, int fftSamples, size_t numBuckets);
    ~LogSpectrograph();

    virtual void AddSamples(float* buffer, unsigned int length, int channels);

    virtual std::pair<float, float> BucketRange(size_t bucket) const;
private:
    LogSpectrograph(const LogSpectrograph& o);
    LogSpectrograph& operator=(const LogSpectrograph& o);

    int sampleRate;
    const int kFFTSamples;
    kiss_fftr_cfg kiss_cfg;

	//arrays to hold samples
	std::vector<kiss_fft_scalar> timedata;
	std::vector<kiss_fft_cpx> freqdata;
};

#endif