#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <boost/thread.hpp>

#include <vector>

class Spectrum
{
public:
    // number of buckets is fft_size / 2
    Spectrum(size_t fft_size, int sample_rate_);

    size_t FFTSize() const { return fft_size; }
    size_t NumBuckets() const { return num_buckets; }
    int SampleRate() const { return sample_rate; }

    float Left (const size_t bucketNum) const;
    float Right(const size_t bucketNum) const;
    float Mix  (const size_t bucketNum) const;
protected:
    float& SetLeft (size_t bucketNum);
    float& SetRight(size_t bucketNum);
    void Clear();
private:
    std::vector<float> left_data;
    std::vector<float> right_data;

    size_t fft_size;
	size_t num_buckets;
    int sample_rate;

	mutable boost::mutex mutex;
};

#endif