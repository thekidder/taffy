#ifndef STATE_DEFAULT
#define STATE_DEFAULT

#include "beat_detector.h"
#include "debug_gui.h"
#include "kiss_spectrum.h"
#include "energy_analyzer.h"
#include "fmod_spectrum.h"
#include "fmod_pass_through_adapter.h"
#include "geometric_spectrum.h"
#include "graph_visualizer.h"
#include "partial_spectrum.h"
#include "particle_system.h"
#include "sound_pass_through_buffer.h"
#include "spectrum_visualizer.h"

#include <sore_texture2d_loader.h>
#include <sore_glslshader_loader.h>
#include <sore_font_loader.h>
#include <sore_font.h>
#include <sore_gamestate_stack.h>
#include <sore_immediatemodeprovider.h>
#include <sore_inputdistributor.h>
#include <sore_noncopyable.h>
#include <sore_pipeline_renderer.h>
#include <sore_resourcecache.h>
#include <sore_topwidget.h>

namespace FMOD
{
    class Channel;
    class DSP;
    class Sound;
    class System;
};

class FMOD_Spectrum;

class DefaultState : public SORE_Game::Gamestate, SORE_Utility::Noncopyable
{
public:
    DefaultState(SORE_Game::GamestateStack& stack);
    ~DefaultState();

    void Frame(int elapsed);
    void Render();
    const char* GetName() const {return "Default state";}

    virtual bool OnEvent(const SORE_Kernel::Event& e); 
private:
    bool HandleKeyboard(const SORE_Kernel::Event& e);
    bool HandleResize(const SORE_Kernel::Event& e);
    void Quit();

    SORE_Graphics::camera_info GetCamera();
    void GotSamples(float* buffer, unsigned int length, int channels);

    SORE_Graphics::PipelineRenderer renderer;
    SORE_Kernel::InputDistributor distributor;

    // widgets
    SORE_GUI::TopWidget top;
    GraphVisualizer* beat_visualizer_low;
    GraphVisualizer* beat_visualizer_mid;
    GraphVisualizer* beat_visualizer_high;
    GraphVisualizer* energy_visualizer;
    SpectrumVisualizer* spectrum_visualizer;
    DebugGUI* debug;

    // sound things
    SoundPassThroughBuffer buffer;
    FMODPassThroughAdapter fmod_adapter;

    FMOD::System* system;
    FMOD::Sound* sound;
    FMOD::Channel* channel;
#ifdef USE_KISS
    FMOD::DSP* listener;
#endif

    // spectrums
    FMOD_Spectrum fmod_spectrum;
#ifdef USE_KISS
    KISS_Spectrum kiss_spectrum;
#endif
    GeometricSpectrum log_spectrum;

    PartialSpectrum low, mid, high;

    // analyzers
    BeatDetector beat_detector_low;
    BeatDetector beat_detector_mid;
    BeatDetector beat_detector_high;

    EnergyAnalyzer energy_analyzer;
};

#endif
