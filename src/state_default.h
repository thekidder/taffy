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

#include <sore_texture2d_loader.h>
#include <sore_glslshader_loader.h>
#include <sore_font_loader.h>
#include <sore_font.h>
#include <sore_gamestate_stack.h>
#include <sore_immediatemodeprovider.h>
#include <sore_inputdistributor.h>
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

class DefaultState : public SORE_Game::Gamestate
{
public:
    DefaultState(SORE_Game::GamestateStack& stack);
    ~DefaultState();

    void Frame(int elapsed);
    const char* GetName() const {return "Default state";}

    virtual bool OnEvent(const SORE_Kernel::Event& e); 
private:
    bool HandleKeyboard(const SORE_Kernel::Event& e);
    bool HandleResize(const SORE_Kernel::Event& e);
    void Quit();

    SORE_Graphics::PipelineRenderer renderer;
    SORE_Kernel::InputDistributor distributor;

    SORE_FileIO::PackageCache package_cache;
    SORE_Resource::ResourceCache<std::string, SORE_Resource::Texture2D, SORE_Resource::Texture2DLoader> texture_cache;
    SORE_Resource::ResourceCache<std::string, SORE_Resource::GLSLShader, SORE_Resource::GLSLShaderLoader> shader_cache;
    SORE_Resource::ResourceCache<std::string, SORE_Resource::Font, SORE_Resource::FontLoader> font_cache;

    SORE_Graphics::camera_info GetCamera();

    void GotSamples(float* buffer, unsigned int length, int channels);

    SORE_Resource::FontPtr face;
    SORE_Resource::Texture2DPtr particle_texture;

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
