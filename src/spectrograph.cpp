#include "spectrograph.h"

Spectrograph::Spectrograph(size_t numBuckets_) : buckets(numBuckets_), numBuckets(numBuckets_)
{
}

float Spectrograph::Get(size_t bucketNum) const
{
	boost::unique_lock<boost::mutex> lock(mutex);
    return buckets[bucketNum];
}

float& Spectrograph::Set(size_t bucketNum)
{
	boost::unique_lock<boost::mutex> lock(mutex);
    return buckets[bucketNum];
}