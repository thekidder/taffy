#ifndef SOUND_PASS_THROUGH_BUFFER_H
#define SOUND_PASS_THROUGH_BUFFER_H

#include <boost/function.hpp>

class SoundPassThroughBuffer
{
public:
    // the callback function is called once we have callback_length_ samples of data
    SoundPassThroughBuffer(int callback_length_, int channels_);
    ~SoundPassThroughBuffer();

    // adds length * channels float samples to the internal buffer
    void AddSamples(float* input, unsigned int length, int channels_);

    typedef boost::function<void (float*, unsigned int, int)> Sound_callback_t;
    void SetCallback(Sound_callback_t c);
private:
    // copying does not make sense
    SoundPassThroughBuffer(const SoundPassThroughBuffer& o);
    SoundPassThroughBuffer& operator=(const SoundPassThroughBuffer& o);

    int callback_length; // number of samples
    int channels;
    int buffer_add_pos; // number of floats (samples * channels)
    float* buffer;

    Sound_callback_t callback;
};

#endif