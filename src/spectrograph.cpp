#include "spectrograph.h"

Spectrograph::Spectrograph(size_t numBuckets) : buckets(numBuckets)
{
}

float Spectrograph::Get(size_t bucketNum) const
{
    return buckets[bucketNum];
}

float& Spectrograph::Set(size_t bucketNum)
{
    return buckets[bucketNum];
}