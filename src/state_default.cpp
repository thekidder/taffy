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

#include <algorithm>
#include <cassert>

using SORE_GUI::SVec;
using SORE_GUI::SUnit;

const int k_fft_samples = 2048;
const int k_num_channels = 2;
const int k_num_particles = 5000;

void CreateDisc(Particle& p);

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
      energy_analyzer(&log_spectrum),
      rotating(false), particles(k_num_particles), paused(false),
      imm_mode(SORE_Resource::Texture2DPtr(), SORE_Resource::GLSLShaderPtr())
{
    gamestateStack.PackageCache().AddPackage("ix_style.sdp");
    gamestateStack.PackageCache().AddPackage("default_resources.sdp");

    // gui gets to look at all events first
    distributor.AddListener(
        SORE_Kernel::INPUT_ALL | SORE_Kernel::RESIZE,
        boost::bind(&SORE_GUI::TopWidget::PropagateEvents, boost::ref(top), _1));

    distributor.AddListener(
        SORE_Kernel::KEYDOWN,
         boost::bind(&DefaultState::HandleKeyboard, this, _1));
    distributor.AddListener(
        SORE_Kernel::INPUT_ALLMOUSE,
         boost::bind(&DefaultState::HandleMouse, this, _1));
    distributor.AddListener(
        SORE_Kernel::RESIZE,
        boost::bind(&DefaultState::HandleResize, this, _1));
    distributor.AddListener(
        SORE_Kernel::RESIZE,
        boost::bind(&ParticleSystem::OnResize, boost::ref(particles), _1));
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

    SORE_GUI::Widget* container2 = new SORE_GUI::Widget(SVec(SUnit(1.0), SUnit(1.0)), SVec(), &top);
    SORE_GUI::Widget* container = new SORE_GUI::Widget(SVec(SUnit(1.0), SUnit(1.0)), SVec(), container2);

    SORE_GUI::FrameWindow* displays_controls = new SORE_GUI::FrameWindow(
        SVec(SUnit(200), SUnit(120)), SVec(SUnit(1.0, -210), SUnit(36)),
        "Visibility Controls", container);

    SORE_GUI::Checkbox* visualizers_controls = new SORE_GUI::Checkbox(SUnit(16), SVec(SUnit(5), SUnit()), displays_controls);
    visualizers_controls->SetChecked();
    new SORE_GUI::TextWidget(SUnit(16), SVec(SUnit(26), SUnit()), displays_controls, "All Displays");

    SORE_GUI::Widget* controls_container = new SORE_GUI::Widget(SVec(SUnit(1.0, -16), SUnit(60)), SVec(SUnit(16), SUnit(20)), displays_controls);

    SORE_GUI::Checkbox* beats_controls = new SORE_GUI::Checkbox(SUnit(16), SVec(SUnit(5), SUnit()), controls_container);
    beats_controls->SetChecked();
    new SORE_GUI::TextWidget(SUnit(16), SVec(SUnit(26), SUnit()), controls_container, "Beat detectors");

    SORE_GUI::Checkbox* energy_controls = new SORE_GUI::Checkbox(SUnit(16), SVec(SUnit(5), SUnit(20)), controls_container);
    energy_controls->SetChecked();
    new SORE_GUI::TextWidget(SUnit(16), SVec(SUnit(26), SUnit(20)), controls_container, "Energy Analyzer");

    SORE_GUI::Checkbox* spectrum_controls = new SORE_GUI::Checkbox(SUnit(16), SVec(SUnit(5), SUnit(40)), controls_container);
    spectrum_controls->SetChecked();
    new SORE_GUI::TextWidget(SUnit(16), SVec(SUnit(26), SUnit(40)), controls_container, "Frequency Spectrum");

    SORE_GUI::Widget* displays = new SORE_GUI::Widget(SVec(SUnit(1.0), SUnit(1.0)), SVec(), &top);

    beat_visualizer_low  = new GraphVisualizer(SVec(SUnit(1.0), SUnit(0.175)), SVec(SUnit(), SUnit(0.0125 + 0.2 * 0)), displays, 
        beat_detector_low.Range(), beat_detector_low.NumValues(), 500);
    beat_visualizer_mid  = new GraphVisualizer(SVec(SUnit(1.0), SUnit(0.175)), SVec(SUnit(), SUnit(0.0125 + 0.2 * 1)), displays, 
        beat_detector_mid.Range(), beat_detector_mid.NumValues(), 500);
    beat_visualizer_high = new GraphVisualizer(SVec(SUnit(1.0), SUnit(0.175)), SVec(SUnit(), SUnit(0.0125 + 0.2 * 2)), displays, 
        beat_detector_high.Range(), beat_detector_high.NumValues(), 500);

    energy_visualizer    = new GraphVisualizer(SVec(SUnit(1.0), SUnit(0.175)), SVec(SUnit(), SUnit(0.0125 + 0.2 * 3)), displays, 
        energy_analyzer.Range(), energy_analyzer.NumValues(), 500);

    spectrum_visualizer  = new SpectrumVisualizer(SVec(SUnit(1.0), SUnit(0.175)), SVec(SUnit(), SUnit(0.0125 + 0.2 * 4)), displays, 0);

    debug = new DebugGUI(renderer, container);

    visualizers_controls->ConnectChecked(boost::bind(&SORE_GUI::Widget::SetVisible, displays, _1));

    beats_controls->ConnectChecked(boost::bind(&SORE_GUI::Widget::SetVisible, beat_visualizer_low, _1));
    beats_controls->ConnectChecked(boost::bind(&SORE_GUI::Widget::SetVisible, beat_visualizer_mid, _1));
    beats_controls->ConnectChecked(boost::bind(&SORE_GUI::Widget::SetVisible, beat_visualizer_high, _1));

    energy_controls->ConnectChecked(boost::bind(&SORE_GUI::Widget::SetVisible, energy_visualizer, _1));

    spectrum_controls->ConnectChecked(boost::bind(&SORE_GUI::Widget::SetVisible, spectrum_visualizer, _1));

    visualizers_controls->SetChecked(false);

    renderer.AddGeometryProvider(&particles);
    renderer.AddGeometryProvider(top.GetGeometryProvider());
    renderer.AddGeometryProvider(&imm_mode);

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

    beat_visualizer_low->SetComment((boost::format("%.1f - %.1f Hz") % low.TotalHz().first % low.TotalHz().second).str());
    beat_visualizer_mid->SetComment((boost::format("%.1f - %.1f Hz") % mid.TotalHz().first % mid.TotalHz().second).str());
    beat_visualizer_high->SetComment((boost::format("%.1f - %.1f Hz") % high.TotalHz().first % high.TotalHz().second).str());

    particles.SetTexture(gamestateStack.TextureCache().Get("particle.tga"));
    particles.SetShader(gamestateStack.ShaderCache().Get("particles.shad"));

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
	glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
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

    if(paused)
        return;

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

    particles.Update(elapsed);

    // draw gui
    top.Frame(elapsed);

    imm_mode.Start();
    imm_mode.SetShader(gamestateStack.ShaderCache().Get("untextured.shad"));
    imm_mode.SetKeywords("game");

    // draw some axes

    const float AXIS_LENGTH = 1.5f;

    imm_mode.SetColor(SORE_Graphics::Green);
    imm_mode.DrawLine(0.0f, 0.0f, 0.0f, AXIS_LENGTH, 0.0f, 0.0f);
    imm_mode.SetColor(SORE_Graphics::Red);
    imm_mode.DrawLine(0.0f, 0.0f, 0.0f, 0.0f, AXIS_LENGTH, 0.0f);
    imm_mode.SetColor(SORE_Graphics::Blue);
    imm_mode.DrawLine(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, AXIS_LENGTH);

    // do the magic
    float beat = static_cast<float>(beat_detector_low.Beat());
    int bass_particles = static_cast<int>(beat * 100.0f);
    bass_particles = std::min(k_num_particles, bass_particles);
    particles.AddParticles(&CreateDisc, bass_particles);

    particles.SetSize(static_cast<float>(energy_analyzer.Energy(0)) * 0.01f);
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
        case SORE_Kernel::Key::SSYM_p:
            paused = !paused;
            channel->setPaused(paused);
            return true;
        case SORE_Kernel::Key::SSYM_d:
            particles.AddParticles(&CreateDisc, 1200);
            return true;
        default:
            break;
        }
    }
    return false;
}

bool DefaultState::HandleMouse(const SORE_Kernel::Event& e)
{
    switch(e.type)
    {
    case SORE_Kernel::MOUSEMOVE:
        if(rotating)
        {
            // rotate 2pi every half screen
            float x = static_cast<float>(e.mouse.xmove) / width;
            float y = static_cast<float>(e.mouse.ymove) / height;

            x *= 4 * static_cast<float>(M_PI);
            y *= 4 * static_cast<float>(M_PI);

            camera.Rotate(x, y);
            return true;
        }
        break;
    case SORE_Kernel::MOUSEBUTTONDOWN:
        rotating = true;
        return true;
    case SORE_Kernel::MOUSEBUTTONUP:
        rotating = false;
        return true;
    default:
        break;
    }
    return false;
}

bool DefaultState::HandleResize(const SORE_Kernel::Event& e)
{
    width = e.resize.w;
    height = e.resize.h;
    return true;
}

SORE_Graphics::camera_info DefaultState::GetCamera()
{
    SORE_Graphics::ProjectionInfo proj;
    proj.type = SORE_Graphics::PERSPECTIVE;
    proj.fov = 45.0f;
    proj.znear = 1.0f;
    proj.zfar = 100.0f;
    proj.useScreenRatio = true;

    SORE_Math::Matrix4<float> view = camera.Matrix();
    SORE_Graphics::camera_info cam = {proj, view};
    return cam;
}

void DefaultState::GotSamples(float* buffer, unsigned int length, int channels)
{
#ifdef USE_KISS
    kiss_spectrum.AddSamples(buffer, length, channels);
#endif
}

void CreateDisc(Particle& p)
{
    float angle = SORE_Utility::getRandomMinMax(0.0f, static_cast<float>(2 * M_PI));
    float dist = SORE_Utility::getRandomMinMax(0.0f, 0.05f);

    p.size = 0.2f;

    p.x = cos(angle) * dist;
    p.y = SORE_Utility::getRandomMinMax(-0.02f, 0.02f);
    p.z = sin(angle) * dist;

    angle = SORE_Utility::getRandomMinMax(0.0f, static_cast<float>(2 * M_PI));
    float speed = SORE_Utility::getRandomMinMax(0.1f, 1.6f);

    p.xv = cos(angle) * speed;
    p.yv = SORE_Utility::getRandomMinMax(-0.004f, 0.004f);
    p.zv = sin(angle) * speed;

    p.xa = -p.xv / 100.0f;
    p.za = -p.zv / 100.0f;

    p.color = SORE_Graphics::White;
    p.colorChange = SORE_Graphics::Color(0.0f, 0.0f, 0.0f, -0.085f);

    p.lifetime = 0.0f;
}