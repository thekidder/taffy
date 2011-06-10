#include "spectrum_snapshot.h"

#include <cassert>
#include <limits>
#include <stdexcept>

SpectrumSnapshot::SpectrumSnapshot(std::vector<float> left_data, std::vector<float> right_data)
    : left(left_data), right(right_data), num_buckets(left.size())
{
    //channels must be of the same size
    assert(left.size() == right.size());
}

SpectrumSnapshot::SpectrumSnapshot(size_t num_buckets_) // construct an empty snapshot
    : num_buckets(num_buckets_)
{
}

float SpectrumSnapshot::Value(size_t bucket, Audio_channel channel) const
{
    // if we have an empty snapshot, return a default value
    // of no sound (-inf dB)
    if(left.size() == 0)
        return -std::numeric_limits<float>::max();

    switch(channel)
    {
    case CHANNEL_LEFT:
        return left[bucket];
    case CHANNEL_RIGHT:
        return right[bucket];
    case STEREO_MIX:
        return (left[bucket] + right[bucket]) / 2.0f;
    default:
        throw std::runtime_error("Invalid channel ID accessed");
    }

    return -std::numeric_limits<float>::max();
}
