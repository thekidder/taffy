#ifndef SPECTRUM_SNAPSHOT_H
#define SPECTRUM_SNAPSHOT_H

#include <vector>

enum Audio_channel
{
    CHANNEL_LEFT,
    CHANNEL_RIGHT,
    STEREO_MIX
};

// immutable class representing a single snapshot in time of
// an audio spectrum
class SpectrumSnapshot
{
public:
    SpectrumSnapshot(std::vector<double> left_data, std::vector<double> right_data);
    SpectrumSnapshot(size_t num_buckets_); // construct an empty snapshot

    size_t NumBuckets() const { return num_buckets; }
    double Value(size_t bucket, Audio_channel channel) const;
private:
    std::vector<double> left, right;
    size_t num_buckets;
};

#endif