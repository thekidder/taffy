#include "app_log.h"
#include "fmod_spectrum.h"
#include "state_default.h"
#include "utility.h"

#include <sore_gamestate_manager.h>
#include <sore_util.h>

#include <boost/bind.hpp>
#include <fmod.hpp>

#include <cassert>

using SORE_GUI::SVec;
using SORE_GUI::SUnit;

const int kFFTSamples = 1024;
const int kNumChannels = 2;

DefaultState::DefaultState() 
    : Gamestate(20), // run every 20 milliseconds
    top(0), debug(0), buffer(kFFTSamples * kNumChannels, kNumChannels), fmod_adapter(buffer),
    use_kiss(false), use_original(false), last_frame(0),
    last_beat(0.0f), imm_mode(SORE_Graphics::Texture2DPtr(), SORE_Graphics::GLSLShaderPtr())
{
}

DefaultState::~DefaultState()
{
    SetAdapter(0);

    delete particles;
    delete fmod_g_spectrum;
    delete kiss_g_spectrum;
    delete fmod_spectrum;
    delete kiss_spectrum;
    delete debug;
    delete top;
    
    system->release();
}

void DefaultState::Init()
{
    owner->GetInputTask().AddListener(SORE_Kernel::KEYDOWN,
                                       boost::bind(&DefaultState::HandleKeyboard, this, _1));
    owner->GetInputTask().AddListener(SORE_Kernel::RESIZE,
                                       boost::bind(&DefaultState::HandleResize, this, _1));

    top = new gui::TopWidget(owner->GetRenderer()->GetScreenInfo().width,
                             owner->GetRenderer()->GetScreenInfo().height);

    SORE_Kernel::InputTask& input = owner->GetInputTask();
    input.AddListener(SORE_Kernel::INPUT_ALL | SORE_Kernel::RESIZE,
                       std::bind1st(std::mem_fun(
                                        &SORE_GUI::TopWidget::PropagateEvents), top));
    input.AddListener(SORE_Kernel::RESIZE ,
                       std::bind1st(std::mem_fun(&gui::TopWidget::OnResize), top));

    beat_visualizer = new GraphVisualizer(SVec(SUnit(1.0, 0), SUnit(0.25, 0)), SVec(SUnit(0.0, 0), SUnit(0.25, 0)), top, owner->GetPool(), std::make_pair(0.0f, 30.0f), 4, 500);
    spectrum_visualizer = new SpectrumVisualizer(SVec(SUnit(1.0, 0), SUnit(0.25, 0)), SVec(SUnit(0.0, 0), SUnit(0.75, 0)), top, owner->GetPool(), 0);

    debug = new DebugGUI(owner->GetRenderer(), owner->GetPool(),
                         owner->GetInputTask(), top);

    face = owner->GetPool().GetResource<SORE_Font::Font>("data/ix_style/LiberationSans-Regular.ttf");

    SORE_Graphics::GLSLShaderPtr point_sprite =
        owner->GetPool().GetResource<SORE_Graphics::GLSLShader>("data/Shaders/point_sprite.shad");
    SORE_Graphics::GLSLShaderPtr default_shader =
        owner->GetPool().GetResource<SORE_Graphics::GLSLShader>("data/Shaders/default.shad");
    particle_texture =
        owner->GetPool().GetResource<SORE_Graphics::Texture2D>("data/Textures/particle.tga");
    particles = new ParticleSystem(particle_texture, point_sprite);

    imm_mode.SetShader(default_shader);
    imm_mode.SetTexture(particle_texture);

    //owner->GetRenderer()->AddGeometryProvider(particles);
    owner->GetRenderer()->AddGeometryProvider(&imm_mode);
    owner->GetRenderer()->AddGeometryProvider(top->GetGeometryProvider());

    SORE_Graphics::camera_callback guiCam = boost::bind(
        &SORE_GUI::TopWidget::GetCamera,
        boost::ref(top));

    SORE_Graphics::camera_callback normalCam = boost::bind(
        &DefaultState::GetCamera,
        this);

    SORE_Graphics::camera_callback_table cameras;
    cameras["gui"] = guiCam;
    cameras["normal"] = normalCam;
    owner->GetRenderer()->SetCameraTable(cameras);

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

    fmod_spectrum = new FMOD_Spectrum(kFFTSamples, sample_rate, system);
    kiss_spectrum = new KISS_Spectrum(kFFTSamples, sample_rate);

    fmod_g_spectrum = new GeometricSpectrum(*fmod_spectrum, 10);
    kiss_g_spectrum = new GeometricSpectrum(*kiss_spectrum, 10);

    spectrum_visualizer->SetSpectrum(fmod_g_spectrum);
}

void DefaultState::Frame(int elapsed)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
    glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_LINE_SMOOTH); 
    glLineWidth(1);

    fmod_spectrum->Update();
    system->update();

    particles->ClearParticles();

    Spectrum* spectrum;
    if(use_kiss)
    {
        if(use_original)
            spectrum = kiss_spectrum;
        else
            spectrum = kiss_g_spectrum;
    }
    else
    {
        if(use_original)
            spectrum = fmod_spectrum;
        else
            spectrum = fmod_g_spectrum;
    }
    spectrum_visualizer->SetSpectrum(spectrum);

    // only compute flux when this frame and the last frame have the same # of buckets
    // i.e. don't do this computation when we change spectrums, could result in a crash
    if(last_frame.NumBuckets() == spectrum->NumBuckets())
    {
        double flux = 0.0f;
        for(size_t i = 0; i < spectrum->NumBuckets(); ++i)
        {
            float diff = spectrum->Value(i, STEREO_MIX) - last_frame.Value(i, STEREO_MIX);
            diff = diff > 0.0f ? diff : 0.0f;
            flux += diff;
        }
        flux /= spectrum->NumBuckets();

        if(flux_history.size() == 11)
            flux_history.pop_front();
        flux_history.push_back(flux);

        float flux_moving_average = 0;
        for(std::list<float>::reverse_iterator it = flux_history.rbegin(); it != flux_history.rend(); ++it)
        {
            flux_moving_average += *it;
        }
        if(flux_history.size())
            flux_moving_average /= flux_history.size();

        float threshold = 1.5f * flux_moving_average;
        float beat = flux - threshold < 0 ? 0.0f : flux - threshold;
        float detected = 0.0f;
        if(beat > last_beat)
            detected = beat;
        
        beat_visualizer->AddDatum(0, flux);
        beat_visualizer->AddDatum(1, threshold);
        beat_visualizer->AddDatum(2, beat);
        beat_visualizer->AddDatum(3, 30.0f - detected);

        last_beat = beat;
    }

    last_frame = spectrum->Snapshot();

    // draw gui
    top->Frame(elapsed);
}

void DefaultState::Quit()
{
    owner->PopState();
}

bool DefaultState::HandleKeyboard(SORE_Kernel::Event* e)
{
    if(e->type == SORE_Kernel::KEYDOWN)
    {
        switch(e->key.keySym)
        {
        case SDLK_ESCAPE:
            Quit();
            return true;
        case SDLK_r:
            fmod_spectrum->SetWindowType(FMOD_DSP_FFT_WINDOW_RECT);
            return true;
        case SDLK_t:
            fmod_spectrum->SetWindowType(FMOD_DSP_FFT_WINDOW_TRIANGLE);
            return true;
        case SDLK_m:
            fmod_spectrum->SetWindowType(FMOD_DSP_FFT_WINDOW_HAMMING);
            return true;
        case SDLK_n:
            fmod_spectrum->SetWindowType(FMOD_DSP_FFT_WINDOW_HANNING);
            return true;
        case SDLK_k:
            use_kiss = !use_kiss;
            return true;
        case SDLK_o:
            use_original = !use_original;
            return true;
        }
    }
    return false;
}

bool DefaultState::HandleResize(SORE_Kernel::Event* e)
{
    particles->Regenerate();
    return true;
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
    kiss_spectrum->AddSamples(buffer, length, channels);
}
