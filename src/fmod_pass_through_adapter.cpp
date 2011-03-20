#include "fmod_pass_through_adapter.h"

#include <cassert>

FMODPassThroughAdapter::FMODPassThroughAdapter(SoundPassThroughBuffer& sptb) : buffer(sptb)
{
}

FMOD_RESULT F_CALLBACK FMODPassThroughAdapter::operator()(FMOD_DSP_STATE* dsp_state, float* in_buffer, float* out_buffer, unsigned int length, int in_channels, int out_channels)
{
    assert(in_channels == out_channels);

    memcpy(out_buffer, in_buffer, length * in_channels * 4);
    buffer.AddSamples(in_buffer, length, in_channels);

    return FMOD_OK;
}

static FMODPassThroughAdapter* adapter = NULL;

void SetAdapter(FMODPassThroughAdapter* adapter_)
{
    adapter = adapter_;
}

FMOD_RESULT F_CALLBACK DSPCallback(FMOD_DSP_STATE* dsp_state, float* in_buffer, float* out_buffer, unsigned int length, int in_channels, int out_channels)
{
    if(adapter)
        return (*adapter)(dsp_state, in_buffer, out_buffer, length, in_channels, out_channels);
    else
        return FMOD_ERR_PLUGIN; // no adapter!
}