#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H

#include <boost/thread.hpp>

#include <vector>

class Spectrograph
{
public:
    Spectrograph(size_t numBuckets_);

    virtual void AddSamples(float* buffer, unsigned int length, int channels) = 0;

    size_t NumBuckets() const { return numBuckets; }
    virtual std::pair<float, float> BucketRange(size_t bucket) const = 0;
    float Get(const size_t bucketNum) const;
protected:
    float& Set(size_t bucketNum);
    void Clear() { boost::unique_lock<boost::mutex> lock(mutex); buckets.clear();  buckets.resize(numBuckets); }
private:
    std::vector<float> buckets;
	size_t numBuckets;

	mutable boost::mutex mutex;
};

#endif