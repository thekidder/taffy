#include "spectrum.h"

#include <cassert>

Spectrum::Spectrum(size_t fft_size_, int sample_rate_) 
    : left_data(fft_size_ / 2), right_data(fft_size_ / 2), fft_size(fft_size_), num_buckets(fft_size_ / 2), sample_rate(sample_rate_)
{
    // fft size must be power of two
    assert(fft_size && ((fft_size & (fft_size - 1)) == 0));
}

float Spectrum::Left(size_t bucket_num) const
{
    assert(bucket_num < num_buckets);
	boost::unique_lock<boost::mutex> lock(mutex);
    return left_data[bucket_num];
}

float Spectrum::Right(size_t bucket_num) const
{
    assert(bucket_num < num_buckets);
	boost::unique_lock<boost::mutex> lock(mutex);
    return right_data[bucket_num];
}

float Spectrum::Mix(size_t bucket_num) const
{
    assert(bucket_num < num_buckets);
	boost::unique_lock<boost::mutex> lock(mutex);
    return (left_data[bucket_num] + right_data[bucket_num]) / 2.0f;
}

float& Spectrum::SetLeft(size_t bucket_num)
{
    assert(bucket_num < num_buckets);
	boost::unique_lock<boost::mutex> lock(mutex);
    return left_data[bucket_num];
}

float& Spectrum::SetRight(size_t bucket_num)
{
    assert(bucket_num < num_buckets);
	boost::unique_lock<boost::mutex> lock(mutex);
    return right_data[bucket_num];
}

void Spectrum::Clear()
{
    boost::unique_lock<boost::mutex> lock(mutex);
    left_data.clear();
    left_data.resize(num_buckets);
    right_data.clear();
    right_data.resize(num_buckets);
}