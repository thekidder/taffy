#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H

#include <vector>

class Spectrograph
{
public:
    Spectrograph(size_t numBuckets);

    virtual void AddSamples(float* buffer, unsigned int length, int channels) = 0;

    size_t NumBuckets() const { return buckets.size(); }
    virtual std::pair<float, float> BucketRange(size_t bucket) const = 0;
    float Get(const size_t bucketNum) const;
protected:
    float& Set(size_t bucketNum);
    void Clear() { buckets.clear(); }
private:
    std::vector<float> buckets;
};

#endif