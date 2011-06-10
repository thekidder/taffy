#ifndef STATE_DEFAULT
#define STATE_DEFAULT

#include "beat_detector.h"
#include "debug_gui.h"
#include "kiss_spectrum.h"
#include "fmod_pass_through_adapter.h"
#include "geometric_spectrum.h"
#include "graph_visualizer.h"
#include "partial_spectrum.h"
#include "particle_system.h"
#include "sound_pass_through_buffer.h"
#include "spectrum_visualizer.h"

#include <sore_font.h>
#include <sore_gamestate.h>
#include <sore_immediatemodeprovider.h>
#include <sore_input.h>
#include <sore_topwidget.h>

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

    SORE_Font::FontPtr face;
    SORE_Graphics::Texture2DPtr particle_texture;

    SORE_GUI::TopWidget* top;
    GraphVisualizer* beat_visualizer_low;
    GraphVisualizer* beat_visualizer_mid;
    GraphVisualizer* beat_visualizer_high;
    SpectrumVisualizer* spectrum_visualizer;
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

    PartialSpectrum* low, *mid, *high;

    bool use_kiss;
    bool use_original;

    BeatDetector beat_detector_low;
    BeatDetector beat_detector_mid;
    BeatDetector beat_detector_high;

    ParticleSystem* particles;
    SORE_Graphics::ImmediateModeProvider imm_mode;
};

#endif
