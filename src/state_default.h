#ifndef STATE_DEFAULT
#define STATE_DEFAULT

#include "debug_gui.h"
#include "kiss_spectrum.h"
#include "fmod_pass_through_adapter.h"
#include "immediate_mode_provider.h"
#include "geometric_spectrum.h"
#include "graph_visualizer.h"
#include "particle_system.h"
#include "sound_pass_through_buffer.h"

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

class FMOD_Spectrum;

class DefaultState : public SORE_Game::Gamestate
{
public:
    DefaultState();
    ~DefaultState();

    void Frame(int elapsed);
    const char* GetName() const {return "Default state";}
private:
    bool HandleKeyboard(SORE_Kernel::Event* e);
    bool HandleResize(SORE_Kernel::Event* e);
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

    KISS_Spectrum* kiss_spectrum;
    FMOD_Spectrum* fmod_spectrum;

    GeometricSpectrum* kiss_g_spectrum;
    GeometricSpectrum* fmod_g_spectrum;

    bool use_kiss;
    bool use_original;

    SpectrumSnapshot last_frame;
    GraphVisualizer beat_visualizer;
    std::list<float> flux_history;

    ParticleSystem* particles;
    ImmediateModeProvider imm_mode;
};

#endif
