#ifndef FMOD_PASS_THROUGH_ADAPTER
#define FMOD_PASS_THROUGH_ADAPTER

#include "sound_pass_through_buffer.h"

#include <fmod.hpp>

class FMODPassThroughAdapter
{
public:
    FMODPassThroughAdapter(SoundPassThroughBuffer& sptb);

    FMOD_RESULT F_CALLBACK operator()(FMOD_DSP_STATE* dsp_state, float* in_buffer, float* out_buffer, unsigned int length, int in_channels, int out_channels);
private:
    SoundPassThroughBuffer& buffer;
};

void SetAdapter(FMODPassThroughAdapter* adapter_);
// uses adapter set through the previous function
FMOD_RESULT F_CALLBACK DSPCallback(FMOD_DSP_STATE* dsp_state, float* in_buffer, float* out_buffer, unsigned int length, int in_channels, int out_channels);

#endif