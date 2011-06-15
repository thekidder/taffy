// strcpy and MSVC++
#define _CRT_SECURE_NO_WARNINGS

#include "app_log.h"
#include "fmod_spectrum.h"
#include "state_default.h"
#include "utility.h"

#include <sore_checkbox.h>
#include <sore_util.h>

#include <boost/bind.hpp>
#include <fmod.hpp>

#include <cassert>

using SORE_GUI::SVec;
using SORE_GUI::SUnit;

const int k_fft_samples = 2048;
const int k_num_channels = 2;

DefaultState::DefaultState(SORE_Game::GamestateStack& stack)
    : Gamestate(stack, 20), // run every 20 milliseconds
      top(gamestateStack.FontCache(),
          gamestateStack.ShaderCache(),
          gamestateStack.TextureCache()),
      buffer(k_fft_samples * k_num_channels, k_num_channels), fmod_adapter(buffer),
      fmod_spectrum(k_fft_samples, 48000), 
#ifdef USE_KISS
      kiss_spectrum(k_fft_samples, 48000), 
#endif
      log_spectrum(fmod_spectrum, 20),
      low(log_spectrum, 0, 5), mid(log_spectrum, 5, 10), high(log_spectrum, 10, 20),
      beat_detector_low(&low), beat_detector_mid(&mid), beat_detector_high(&high),
      energy_analyzer(&log_spectrum)
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

    energy_analyzer.AddSpectrum(&low);
    energy_analyzer.AddSpectrum(&mid);
    energy_analyzer.AddSpectrum(&high);

    // setup the style
    top.SetStyleName("ix");
    SORE_FileIO::InFile ix_style("data/ix.json", &stack.PackageCache());
    top.LoadStyle(ix_style);

    distributor.AddListener(
        SORE_Kernel::INPUT_ALL | SORE_Kernel::RESIZE,
        boost::bind(&SORE_GUI::TopWidget::PropagateEvents, boost::ref(top), _1));

    SORE_GUI::Widget* container = new SORE_GUI::Widget(SVec(SUnit(1.0), SUnit(1.0)), SVec(), &top);

    SORE_GUI::FrameWindow* displays = new SORE_GUI::FrameWindow(
        SVec(SUnit(200), SUnit(100)), SVec(SUnit(1.0, -210), SUnit(36)),
        "Visibility Controls", container);

    SORE_GUI::Checkbox* visualizers_controls = new SORE_GUI::Checkbox(SUnit(16), SVec(SUnit(5), SUnit()), displays);
    visualizers_controls->SetChecked();
    new SORE_GUI::TextWidget(SUnit(16), SVec(SUnit(26), SUnit()), displays, "Beat detectors");

    SORE_GUI::Checkbox* energy_controls = new SORE_GUI::Checkbox(SUnit(16), SVec(SUnit(5), SUnit(20)), displays);
    energy_controls->SetChecked();
    new SORE_GUI::TextWidget(SUnit(16), SVec(SUnit(26), SUnit(20)), displays, "Energy Analyzer");

    SORE_GUI::Checkbox* spectrum_controls = new SORE_GUI::Checkbox(SUnit(16), SVec(SUnit(5), SUnit(40)), displays);
    spectrum_controls->SetChecked();
    new SORE_GUI::TextWidget(SUnit(16), SVec(SUnit(26), SUnit(40)), displays, "Frequency Spectrum");

    beat_visualizer_low  = new GraphVisualizer(SVec(SUnit(1.0), SUnit(0.175)), SVec(SUnit(), SUnit(0.0125 + 0.2 * 0)), &top, 
        beat_detector_low.Range(), beat_detector_low.NumValues(), 500);
    beat_visualizer_mid  = new GraphVisualizer(SVec(SUnit(1.0), SUnit(0.175)), SVec(SUnit(), SUnit(0.0125 + 0.2 * 1)), &top, 
        beat_detector_mid.Range(), beat_detector_mid.NumValues(), 500);
    beat_visualizer_high = new GraphVisualizer(SVec(SUnit(1.0), SUnit(0.175)), SVec(SUnit(), SUnit(0.0125 + 0.2 * 2)), &top, 
        beat_detector_high.Range(), beat_detector_high.NumValues(), 500);

    energy_visualizer    = new GraphVisualizer(SVec(SUnit(1.0), SUnit(0.175)), SVec(SUnit(), SUnit(0.0125 + 0.2 * 3)), &top, 
        energy_analyzer.Range(), energy_analyzer.NumValues(), 500);

    spectrum_visualizer  = new SpectrumVisualizer(SVec(SUnit(1.0), SUnit(0.175)), SVec(SUnit(), SUnit(0.0125 + 0.2 * 4)), &top, 0);

    debug = new DebugGUI(renderer, container);

    visualizers_controls->ConnectChecked(boost::bind(&SORE_GUI::Widget::SetVisible, beat_visualizer_low, _1));
    visualizers_controls->ConnectChecked(boost::bind(&SORE_GUI::Widget::SetVisible, beat_visualizer_mid, _1));
    visualizers_controls->ConnectChecked(boost::bind(&SORE_GUI::Widget::SetVisible, beat_visualizer_high, _1));

    energy_controls->ConnectChecked(boost::bind(&SORE_GUI::Widget::SetVisible, energy_visualizer, _1));

    spectrum_controls->ConnectChecked(boost::bind(&SORE_GUI::Widget::SetVisible, spectrum_visualizer, _1));

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

#ifdef USE_KISS
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
#endif

    int sample_rate;
    system->getSoftwareFormat(&sample_rate, 0, 0, 0, 0, 0);

    fmod_spectrum.SetSampleRate(sample_rate);
#ifdef USE_KISS
    kiss_spectrum.SetSampleRate(sample_rate);
#endif
    fmod_spectrum.SetFMODSystem(system);

    spectrum_visualizer->SetSpectrum(&log_spectrum);

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
    energy_analyzer.Update();

    for(size_t i = 0; i < beat_detector_low.NumValues(); ++i)
        beat_visualizer_low->AddDatum(i, beat_detector_low.Value(i));

    for(size_t i = 0; i < beat_detector_mid.NumValues(); ++i)
        beat_visualizer_mid->AddDatum(i, beat_detector_mid.Value(i));

    for(size_t i = 0; i < beat_detector_high.NumValues(); ++i)
        beat_visualizer_high->AddDatum(i, beat_detector_high.Value(i));

    for(size_t i = 0; i < energy_analyzer.NumValues(); ++i)
        energy_visualizer->AddDatum(i, energy_analyzer.Value(i));

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
#ifdef USE_KISS
    kiss_spectrum.AddSamples(buffer, length, channels);
#endif
}
