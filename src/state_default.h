#ifndef STATE_DEFAULT
#define STATE_DEFAULT

#include "debug_gui.h"
#include "fmod_pass_through_adapter.h"
#include "particle_system.h"
#include "sound_pass_through_buffer.h"

#include <kiss_fftr.h>
//#include <sndfile.hh>
#include <sore_topwidget.h>
#include <sore_gamestate.h>
#include <sore_input.h>

namespace FMOD
{
    class Channel;
    class DSP;
    class Sound;
    class System;
};

class DefaultState : public SORE_Game::Gamestate
{
public:
    DefaultState();
    ~DefaultState();

    void Frame(int elapsed);
    const char* GetName() const {return "Default state";}
private:
    bool HandleEscapeKey(SORE_Kernel::Event* e);
    void Init();
    void Quit();

    SORE_Graphics::camera_info GetCamera();

    void GotSamples(float* buffer, unsigned int length, int channels);

    SORE_GUI::TopWidget* top;
    DebugGUI* debug;

    SoundPassThroughBuffer buffer;
    FMODPassThroughAdapter fmod_adapter;

    FMOD::System* system;
    FMOD::Sound* sound;
    FMOD::Channel* channel;
    FMOD::DSP* listener;

    kiss_fftr_cfg kiss_cfg;
    //SndfileHandle sound;

    ParticleSystem* particles;

    const static int kNumSpectrumWindows = 20;
    float spectrum[kNumSpectrumWindows];
};

#endif
