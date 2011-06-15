// strcpy and MSVC++
#define _CRT_SECURE_NO_WARNINGS

#include "app_log.h"
#include "fmod_spectrum.h"
#include "state_default.h"
#include "utility.h"

#include <sore_util.h>

#include <boost/bind.hpp>
#include <fmod.hpp>

#include <cassert>

using SORE_GUI::SVec;
using SORE_GUI::SUnit;

const int k_fft_samples = 1024;
const int k_num_channels = 2;

DefaultState::DefaultState(SORE_Game::GamestateStack& stack) 
    : Gamestate(stack, 20), // run every 20 milliseconds
    top(gamestateStack.FontCache(),
        gamestateStack.ShaderCache(),
        gamestateStack.TextureCache()),
    debug(0), buffer(k_fft_samples * k_num_channels, k_num_channels), fmod_adapter(buffer),
    beat_detector_low(0), beat_detector_mid(0), beat_detector_high(0),
    kiss_spectrum(k_fft_samples, 48000), fmod_spectrum(k_fft_samples, 48000),
    kiss_g_spectrum(kiss_spectrum, 24), fmod_g_spectrum(fmod_spectrum, 24),
    low(fmod_g_spectrum, 0, 8), mid(fmod_g_spectrum, 8, 16), high(fmod_g_spectrum, 16, 24)
{
    gamestateStack.PackageCache().AddPackage("ix_style.sdp");
    gamestateStack.PackageCache().AddPackage("default_resources.sdp");

    distributor.AddListener(
        SORE_Kernel::KEYDOWN,
         boost::bind(&DefaultState::HandleKeyboard, this, _1));
    distributor.AddListener(
        SORE_Kernel::RESIZE,
        boost::bind(&DefaultState::HandleResize, this, _1));
    distributor.AddListener(
        SORE_Kernel::RESIZE,
        boost::bind(&SORE_Graphics::PipelineRenderer::OnResize, boost::ref(renderer), _1));

    // setup the style
    top.SetStyleName("ix");
    SORE_FileIO::InFile ix_style("data/ix.json", &stack.PackageCache());
    top.LoadStyle(ix_style);

    distributor.AddListener(
        SORE_Kernel::INPUT_ALL | SORE_Kernel::RESIZE,
        boost::bind(&SORE_GUI::TopWidget::PropagateEvents, boost::ref(top), _1));

    SORE_GUI::Widget* container = new SORE_GUI::Widget(SVec(SUnit(1.0, 0), SUnit(1.0, 0)), SVec(SUnit(0.0, 0), SUnit(0.0, 0)), &top);

    beat_visualizer_low  = new GraphVisualizer(SVec(SUnit(1.0, 0), SUnit(0.2, 0)), SVec(SUnit(0.0, 0), SUnit(0.15, 0)), &top, std::make_pair(0.0f, 30.0f), 4, 500);
    beat_visualizer_mid  = new GraphVisualizer(SVec(SUnit(1.0, 0), SUnit(0.2, 0)), SVec(SUnit(0.0, 0), SUnit(0.4, 0)), &top, std::make_pair(0.0f, 30.0f), 4, 500);
    beat_visualizer_high  = new GraphVisualizer(SVec(SUnit(1.0, 0), SUnit(0.2, 0)), SVec(SUnit(0.0, 0), SUnit(0.65, 0)), &top, std::make_pair(0.0f, 30.0f), 4, 500);

    spectrum_visualizer = new SpectrumVisualizer(SVec(SUnit(1.0, 0), SUnit(0.1, 0)), SVec(SUnit(0.0, 0), SUnit(0.9, 0)), &top, 0);

    debug = new DebugGUI(renderer, container);

    renderer.AddGeometryProvider(top.GetGeometryProvider());

    SORE_Graphics::camera_callback guiCam = boost::bind(
        &SORE_GUI::TopWidget::GetCamera,
        boost::ref(top));

    SORE_Graphics::camera_callback normalCam = boost::bind(
        &DefaultState::GetCamera,
        this);

    SORE_Graphics::camera_callback_table cameras;
    cameras["gui"] = guiCam;
    cameras["normal"] = normalCam;
    renderer.SetCameraTable(cameras);

    FMOD::System_Create(&system);
    system->init(100, FMOD_INIT_NORMAL, 0);
    system->createStream("music.mp3", FMOD_SOFTWARE, 0, &sound);
    system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);

    {
        buffer.SetCallback(boost::bind(&DefaultState::GotSamples, this, _1, _2, _3));
        SetAdapter(&fmod_adapter);

        FMOD_DSP_DESCRIPTION dspdesc;
        memset(&dspdesc, 0, sizeof(FMOD_DSP_DESCRIPTION));

        strcpy(dspdesc.name, "FMOD Listener");
        dspdesc.read = DSPCallback;

        system->createDSP(&dspdesc, &listener);
    }
    listener->setBypass(false);
    system->addDSP(listener, 0);

    int sample_rate;
    system->getSoftwareFormat(&sample_rate, 0, 0, 0, 0, 0);

    kiss_spectrum.SetSampleRate(sample_rate);
    fmod_spectrum.SetSampleRate(sample_rate);
    fmod_spectrum.SetFMODSystem(system);

    spectrum_visualizer->SetSpectrum(&fmod_g_spectrum);
    beat_detector_low.SetSpectrum(&low);
    beat_detector_mid.SetSpectrum(&mid);
    beat_detector_high.SetSpectrum(&high);

    beat_visualizer_low->SetComment((boost::format("%.2f - %.2f Hz") % low.TotalHz().first % low.TotalHz().second).str());
    beat_visualizer_mid->SetComment((boost::format("%.2f - %.2f Hz") % mid.TotalHz().first % mid.TotalHz().second).str());
    beat_visualizer_high->SetComment((boost::format("%.2f - %.2f Hz") % high.TotalHz().first % high.TotalHz().second).str());
}

DefaultState::~DefaultState()
{
    SetAdapter(0);  
    system->release();
}

bool DefaultState::OnEvent(const SORE_Kernel::Event& e)
{
    return distributor.DistributeEvent(e);
}

void DefaultState::Frame(int elapsed)
{
    system->update();
    fmod_spectrum.Update();

    beat_detector_low.Update();
    beat_detector_mid.Update();
    beat_detector_high.Update();

    beat_visualizer_low->AddDatum(0, beat_detector_low.Flux());
    beat_visualizer_low->AddDatum(1, beat_detector_low.Threshold());
    beat_visualizer_low->AddDatum(2, beat_detector_low.ThresholdedFlux());
    beat_visualizer_low->AddDatum(3, 30.0f - beat_detector_low.Beat());
            
    beat_visualizer_mid->AddDatum(0, beat_detector_mid.Flux());
    beat_visualizer_mid->AddDatum(1, beat_detector_mid.Threshold());
    beat_visualizer_mid->AddDatum(2, beat_detector_mid.ThresholdedFlux());
    beat_visualizer_mid->AddDatum(3, 30.0f - beat_detector_mid.Beat());

    beat_visualizer_high->AddDatum(0, beat_detector_high.Flux());
    beat_visualizer_high->AddDatum(1, beat_detector_high.Threshold());
    beat_visualizer_high->AddDatum(2, beat_detector_high.ThresholdedFlux());
    beat_visualizer_high->AddDatum(3, 30.0f - beat_detector_high.Beat());

    // draw gui
    top.Frame(elapsed);
}

void DefaultState::Render()
{
    renderer.Render();
}

void DefaultState::Quit()
{
    gamestateStack.PopState();
}

bool DefaultState::HandleKeyboard(const SORE_Kernel::Event& e)
{
    if(e.type == SORE_Kernel::KEYDOWN)
    {
        switch(e.key.keySym)
        {
        case SORE_Kernel::Key::SSYM_ESCAPE:
            Quit();
            return true;
        case SORE_Kernel::Key::SSYM_r:
            fmod_spectrum.SetWindowType(FMOD_DSP_FFT_WINDOW_RECT);
            return true;
        case SORE_Kernel::Key::SSYM_t:
            fmod_spectrum.SetWindowType(FMOD_DSP_FFT_WINDOW_TRIANGLE);
            return true;
        case SORE_Kernel::Key::SSYM_m:
            fmod_spectrum.SetWindowType(FMOD_DSP_FFT_WINDOW_HAMMING);
            return true;
        case SORE_Kernel::Key::SSYM_n:
            fmod_spectrum.SetWindowType(FMOD_DSP_FFT_WINDOW_HANNING);
            return true;
        default:
            break;
        }
    }
    return false;
}

bool DefaultState::HandleResize(const SORE_Kernel::Event& e)
{
    return false;
}

SORE_Graphics::camera_info DefaultState::GetCamera()
{
    SORE_Graphics::ProjectionInfo proj;
    proj.type = SORE_Graphics::ORTHO2D;
    proj.useScreenRatio = false;
    proj.useScreenCoords = false;
    proj.left = -1.0f;
    proj.right = 1.0f;
    proj.bottom = 1.0f;
    proj.top = -1.0f;
    
    SORE_Math::Matrix4<float> identity;
    SORE_Graphics::camera_info cam = {proj, identity};
    return cam;
}

void DefaultState::GotSamples(float* buffer, unsigned int length, int channels)
{
    kiss_spectrum.AddSamples(buffer, length, channels);
}
