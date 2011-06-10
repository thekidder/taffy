#include "fft_spectrum.h"

#include <cassert>

FFTSpectrum::FFTSpectrum(size_t fft_size_, int sample_rate_) 
    : Spectrum(fft_size_ / 2), current_snapshot(NumBuckets()), fft_size(fft_size_), sample_rate(sample_rate_)
{
    // fft size must be power of two
    assert(fft_size && ((fft_size & (fft_size - 1)) == 0));
}

float FFTSpectrum::Value(size_t bucket, Audio_channel channel) const
{
    assert(bucket < NumBuckets());

    boost::unique_lock<boost::mutex> lock(mutex);
    return current_snapshot.Value(bucket, channel);
}

const SpectrumSnapshot& FFTSpectrum::Snapshot() const
{
    return current_snapshot;
}

void FFTSpectrum::SetSnapshot(const SpectrumSnapshot& new_snapshot)
{
    // spectrum can not change sizes
    assert(new_snapshot.NumBuckets() == NumBuckets());

	boost::unique_lock<boost::mutex> lock(mutex);
    current_snapshot = new_snapshot;
}

void FFTSpectrum::Clear()
{
    boost::unique_lock<boost::mutex> lock(mutex);
    current_snapshot = SpectrumSnapshot(NumBuckets());
}