#ifndef FFT_SPECTRUM_H
#define FFT_SPECTRUM_H

#include "spectrum.h"
#include "spectrum_snapshot.h"

#include <boost/thread.hpp>

class FFTSpectrum : public Spectrum
{
public:
    // number of buckets is fft_size / 2
    FFTSpectrum(size_t fft_size, int sample_rate_);

    size_t FFTSize() const { return fft_size; }
    int SampleRate() const { return sample_rate; }

    void SetSampleRate(int sample_rate_) { sample_rate = sample_rate_; }

    virtual double Value(size_t bucket, Audio_channel channel) const;
    virtual const SpectrumSnapshot& Snapshot() const;

    virtual std::pair<double, double> HzRange(size_t bucket) const;
protected:
    void Clear();
    void SetSnapshot(const SpectrumSnapshot& new_snapshot);
private:
    SpectrumSnapshot current_snapshot;

    size_t fft_size;
    int sample_rate;
    
	mutable boost::mutex mutex;
};

#endif