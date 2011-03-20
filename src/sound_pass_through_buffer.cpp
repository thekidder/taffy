#include "sound_pass_through_buffer.h"

#include <cassert>
#include <cstring>

// size of the internal buffer as related to the requested buffer callback length
const int BUFFER_LEN_MULTIPLIER = 4;

SoundPassThroughBuffer::SoundPassThroughBuffer(int callback_length_, int channels_)
    : callback_length(callback_length_), channels(channels_), buffer_add_pos(0),
    buffer(new float[callback_length * channels * BUFFER_LEN_MULTIPLIER])
{
}

SoundPassThroughBuffer::~SoundPassThroughBuffer()
{
    delete buffer;
}

void SoundPassThroughBuffer::AddSamples(float* input, unsigned int length, int channels_)
{
    if(!callback)
        return; // nobody to notify, why bother

    // does not process differing number of channels
    assert(channels == channels_);
    // make sure we have enough room in our buffer
    assert(length * channels <= (unsigned int)(callback_length * channels * BUFFER_LEN_MULTIPLIER - buffer_add_pos));

    memcpy(buffer + buffer_add_pos, input, length * channels * 4);
    buffer_add_pos += length * channels;

    if(buffer_add_pos >= callback_length * channels)
    {
        callback(buffer, callback_length, channels);
        //move unused samples to beginning of buffer
        int extra_samples = buffer_add_pos - callback_length * channels;
        memcpy(buffer, buffer + callback_length * channels, extra_samples);
        buffer_add_pos = extra_samples;
    }
}

void SoundPassThroughBuffer::SetCallback(Sound_callback_t c)
{
    callback = c;
}