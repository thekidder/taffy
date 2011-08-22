// strcpy and MSVC++
#define _CRT_SECURE_NO_WARNINGS

#include "app_log.h"
#include <sore_matrix4x4.h>
#include "fmod_spectrum.h"

#include "hsv_color.h"
#include "pipes.h"
#include "state_default.h"
#include "utility.h"

#include <sore_checkbox.h>
#include <sore_sample.h>
#include <sore_util.h>

#include <boost/bind.hpp>
#include <fmod.hpp>

#include <algorithm>
#include <cassert>

using SORE_GUI::SVec;
using SORE_GUI::SUnit;

const int k_fft_samples = 2048;
const int k_num_channels = 2;
const int k_num_particles_width  = 1024;
const int k_num_particles_height = 512;

DefaultState::DefaultState(SORE_Game::GamestateStack& stack)
    : Gamestate(stack, 20), // run every 20 milliseconds
      renderer(gamestateStack.Profiler()),
      top(gamestateStack.FontCache(),
          gamestateStack.ShaderCache(),
          gamestateStack.TextureCache(),
          gamestateStack.Profiler()),
      buffer(k_fft_samples * k_num_channels, k_num_channels), fmod_adapter(buffer),
      fmod_spectrum(k_fft_samples, 48000), 
#ifdef USE_KISS
      kiss_spectrum(k_fft_samples, 48000), 
#endif
      log_spectrum(fmod_spectrum, 20),
      low(log_spectrum, 0, 5), mid(log_spectrum, 5, 10), high(log_spectrum, 10, 20),
      beat_detector_low(&low), beat_detector_mid(&mid), beat_detector_high(&high),
      energy_analyzer(&log_spectrum),
      rotating(false), lightPos(0.0f, 20.0f, 0.0f),
      particles(
      boost::bind(&DefaultState::ParticleCubeSpawner, this, _1),
      k_num_particles_width, k_num_particles_height, 
      gamestateStack.TextureCache(), gamestateStack.ShaderCache()), 
      paused(false),
      imm_mode(SORE_Resource::GLSLShaderPtr()), lightT(0.0f)
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
    distributor.AddListener(
        SORE_Kernel::RESIZE,
        boost::bind(&SORE_Graphics::ImmediateModeProvider::OnResize, boost::ref(imm_mode), _1));

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
    new SORE_GUI::TextWidget(SUnit(16), SVec(SUnit(26), SUnit()), controls_container, "Beat Detectors");

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

    debug = new DebugGUI(renderer, gamestateStack.Profiler(), container);

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

    SORE_Graphics::camera_callback lightCam = boost::bind(
        &DefaultState::GetLightCamera,
        this);

    SORE_Graphics::camera_callback particleUpdateCam = boost::bind(
        &ParticleSystem::GetUpdateCamera,
        boost::ref(particles));

    SORE_Graphics::camera_callback_table cameras;
    cameras["gui"] = guiCam;
    cameras["normal"] = normalCam;
    cameras["light"] = lightCam;
    cameras["particle_update"] = particleUpdateCam;
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

    particles.SetEmitter(gamestateStack.ShaderCache().Get("particles_emitter.shad"));

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
	glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

    imm_mode.SetShader(gamestateStack.ShaderCache().Get("untextured.shad"));
    imm_mode.SetKeywords("game");

    // set up render pipeline
    SORE_Graphics::Pipe* sorter = new SORE_Graphics::SortingPipe(SORE_Graphics::renderableSort, gamestateStack.Profiler());

    SORE_Graphics::Pipe* guiPipe = new SORE_Graphics::FilterPipe(SORE_Graphics::KeywordFilter("gui"), gamestateStack.Profiler());
    guiPipe->AddChildPipe(new SORE_Graphics::RenderPipe("gui"));

    SORE_Graphics::Pipe* gamePipe = new SORE_Graphics::FilterPipe(SORE_Graphics::KeywordFilter("game"), gamestateStack.Profiler());
    gamePipe->AddChildPipe(new SORE_Graphics::RenderPipe("normal"));

    SORE_Graphics::Pipe* particlePipe = new SORE_Graphics::FilterPipe(SORE_Graphics::KeywordFilter("particle"), gamestateStack.Profiler());
    SORE_Graphics::Pipe* shadowPipe = new ParticleShadowPipe(gamestateStack.ShaderCache().Get("particles_shadowmap.shad"), 512, gamestateStack.Profiler());
    particlePipe->AddChildPipe(shadowPipe);
    shadowPipe->AddChildPipe(new SORE_Graphics::RenderPipe("light"));

    SORE_Graphics::Pipe* particleUpdatePipe = new SORE_Graphics::FilterPipe(SORE_Graphics::KeywordFilter("particle_update"), gamestateStack.Profiler());
    SORE_Graphics::Pipe* updatePipe = particles.GetUpdatePipe();
    particleUpdatePipe->AddChildPipe(updatePipe);
    updatePipe->AddChildPipe(new SORE_Graphics::RenderPipe("particle_update"));

    SORE_Graphics::Pipe* particleEmitterPipe = new SORE_Graphics::FilterPipe(SORE_Graphics::KeywordFilter("particle_emitter"), gamestateStack.Profiler());
    SORE_Graphics::Pipe* emitterPipe = particles.GetEmitterPipe();
    particleEmitterPipe->AddChildPipe(emitterPipe);
    emitterPipe->AddChildPipe(new SORE_Graphics::RenderPipe("particle_update"));

    renderer.RootPipe()->AddChildPipe(sorter);
    sorter->AddChildPipe(particlePipe);
    sorter->AddChildPipe(gamePipe);
    sorter->AddChildPipe(guiPipe);
    sorter->AddChildPipe(particleEmitterPipe);
    sorter->AddChildPipe(particleUpdatePipe);
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
    PROFILE_BLOCK("All Logic", gamestateStack.Profiler());
    // even when paused, update fmod and take care of the GUI
    system->update();
    
    // draw gui
    top.Frame(elapsed);

    if(paused)
        return;

    //lightT += elapsed / 50000.0f;

    //lightPos[0] = 6.0f * sin(lightT);
    //lightPos[1] = 6.0f * sin(lightT);
    //lightPos[2] = 6.0f * cos(lightT);

    {
        PROFILE_BLOCK("Music analysis algorithms", gamestateStack.Profiler());
        fmod_spectrum.Update();

        beat_detector_low.Update();
        beat_detector_mid.Update();
        beat_detector_high.Update();
        energy_analyzer.Update();
    }

    float lightIntensity = 4.0f;//static_cast<float>(energy_analyzer.Energy(0) / 10.0);

    for(size_t i = 0; i < beat_detector_low.NumValues(); ++i)
        beat_visualizer_low->AddDatum(i, beat_detector_low.Value(i));

    for(size_t i = 0; i < beat_detector_mid.NumValues(); ++i)
        beat_visualizer_mid->AddDatum(i, beat_detector_mid.Value(i));

    for(size_t i = 0; i < beat_detector_high.NumValues(); ++i)
        beat_visualizer_high->AddDatum(i, beat_detector_high.Value(i));

    for(size_t i = 0; i < energy_analyzer.NumValues(); ++i)
        energy_visualizer->AddDatum(i, energy_analyzer.Value(i));

    {
        PROFILE_BLOCK("Immediate mode drawing", gamestateStack.Profiler());
        imm_mode.Start();
        imm_mode.SetKeywords("game");
        imm_mode.SetBlendMode(SORE_Graphics::BLEND_OPAQUE);
        imm_mode.SetShader(gamestateStack.ShaderCache().Get("untextured.shad"));
        // draw some axes
        const float AXIS_LENGTH = 1.5f;

        imm_mode.SetColor(SORE_Graphics::Green);
        imm_mode.DrawLine(0.0f, 0.0f, 0.0f, AXIS_LENGTH, 0.0f, 0.0f);
        imm_mode.SetColor(SORE_Graphics::Red);
        imm_mode.DrawLine(0.0f, 0.0f, 0.0f, 0.0f, AXIS_LENGTH, 0.0f);
        imm_mode.SetColor(SORE_Graphics::Blue);
        imm_mode.DrawLine(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, AXIS_LENGTH);

        imm_mode.SetUniform("lightPos", lightPos);
        imm_mode.SetUniform("lightIntensity", lightIntensity);
    
        imm_mode.SetShader(gamestateStack.ShaderCache().Get("untextured_lit.shad"));
        imm_mode.SetColor(SORE_Graphics::Grey);
        imm_mode.DrawQuad(
            -100.0f, -3.0f, -100.0f,
            -100.0f, -3.0f,  100.0f,
             100.0f, -3.0f, -100.0f,
             100.0f, -3.0f,  100.0f);

        imm_mode.SetShader(gamestateStack.ShaderCache().Get("point_sprite_alphatest.shad"));
        imm_mode.SetColor(SORE_Graphics::White);
        imm_mode.SetTexture(gamestateStack.TextureCache().Get("particle.tga"));
        imm_mode.DrawPoint(lightPos[0], lightPos[1], lightPos[2], 2.0f);
    }

    {
        PROFILE_BLOCK("Particle updating", gamestateStack.Profiler());

        particles.Update(elapsed, imm_mode);

        // do the magic
        float beat = static_cast<float>(beat_detector_low.Beat());
        int bass_particles = static_cast<int>(beat * 400.0f);
        //bass_particles = std::min(k_num_particles, bass_particles);

        beat = static_cast<float>(beat_detector_mid.Beat());
        int mid_particles = static_cast<int>(beat * 20.0f);
        //mid_particles = std::min(k_num_particles / 8, mid_particles);

        //stars.AddParticles(boost::bind(&DefaultState::CreateExplosion, this, _1), mid_particles);
        //particles.AddParticles(boost::bind(&DefaultState::CreateDisc, this, _1), bass_particles);

        //particles.SetSize(static_cast<float>(energy_analyzer.Energy(0)) * 0.01f);
        //stars.SetSize(static_cast<float>(energy_analyzer.Energy(0)) * 0.01f);

        particles.SetUniform("lightPos", lightPos);
        particles.SetUniform("lightIntensity", lightIntensity);

        float lightMatRaw[16] = {
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f
        };
        SORE_Math::Matrix4<float> lightMatrix(lightMatRaw);
        SORE_Graphics::camera_info lightCam = GetLightCamera();
        lightMatrix *= SORE_Math::Matrix4<float>::GetPerspective(
            lightCam.projection.fov, 1.0f, 
            lightCam.projection.znear, lightCam.projection.zfar);
        lightMatrix *= lightCam.viewMatrix;

        particles.SetUniform("lightMatrix", lightMatrix);
        particles.SetUniform("screenSize", SORE_Math::Vector2<float>(static_cast<float>(width), static_cast<float>(height)));
    }

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
    case SORE_Kernel::MOUSEWHEELDOWN:
        camera.Zoom(-1.0f);
        break;
    case SORE_Kernel::MOUSEWHEELUP:
        camera.Zoom(1.0f);
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
    proj.zfar = 300.0f;
    proj.useScreenRatio = true;

    SORE_Math::Matrix4<float> view = camera.Matrix();
    SORE_Graphics::camera_info cam = {proj, view};
    return cam;
}

SORE_Graphics::camera_info DefaultState::GetLightCamera()
{
    SORE_Graphics::ProjectionInfo proj;
    proj.type = SORE_Graphics::PERSPECTIVE;
    proj.fov = 90.0f;
    proj.znear = 1.0f;
    proj.zfar = 300.0f;
    proj.useScreenRatio = false;

    proj.left = proj.top = -10.0f;
    proj.right = proj.bottom = 10.0f;

    SORE_Math::Matrix4<float> view = SORE_Math::Matrix4<float>::GetLookat(
        lightPos,
        SORE_Math::Vector3f(0.0f, 0.0f, 0.0f),
        SORE_Math::Vector3f(0.0f, 0.0f, 1.0f));
    SORE_Graphics::camera_info cam = {proj, view};
    return cam;
}

void DefaultState::GotSamples(float* buffer, unsigned int length, int channels)
{
#ifdef USE_KISS
    kiss_spectrum.AddSamples(buffer, length, channels);
#endif
}

void DefaultState::ParticleCubeSpawner(ParticleSpawn& p)
{
    p.x = SORE_Utility::getRandomMinMax(-1.0f, 1.0f);
    p.y = SORE_Utility::getRandomMinMax(-1.0f, 1.0f);
    p.z = SORE_Utility::getRandomMinMax(-1.0f, 1.0f);
    p.size = SORE_Utility::getRandomMinMax(0.1f, 0.3f);

    p.r = SORE_Utility::getRandomMinMax(0.0f, 120.0f); // h
    p.r = std::min(60.0f, p.r);
    p.g = SORE_Utility::getRandomMinMax(0.5f, 1.0f); // s
    p.b = SORE_Utility::getRandomMinMax(0.5f, 1.0f); // v
    p.lifetime = 1.0f;

    p.data0 = SORE_Utility::getRandomMinMax(-0.5f, 0.5f); // velocity: x
    p.data1 = SORE_Utility::getRandomMinMax(-0.5f, 0.5f); // y
    p.data2 = SORE_Utility::getRandomMinMax(-0.5f, 0.5f); // z
    p.data3 = SORE_Utility::getRandomMinMax(0.005f, 0.02f); //decay
}