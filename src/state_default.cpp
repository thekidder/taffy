#include "app_log.h"
#include "state_default.h"

#include <sore_gamestate_manager.h>

#include <boost/bind.hpp>
#include <fmod.hpp>

#include <cassert>

const int kFFTSamples = 512;
const int kNumChannels = 2;

DefaultState::DefaultState() 
    : top(0), debug(0), buffer(kFFTSamples * kNumChannels, kNumChannels), fmod_adapter(buffer)
{
}

DefaultState::~DefaultState()
{
    SetAdapter(0);

    delete particles;
    delete debug;
    delete top;
    kiss_fftr_free(kiss_cfg);
    system->release();
}

void DefaultState::Init()
{
    owner->GetInputTask().AddListener(SORE_Kernel::KEYDOWN,
                                       boost::bind(&DefaultState::HandleEscapeKey, this, _1));

    top = new gui::TopWidget(owner->GetRenderer()->GetScreenInfo().width,
                             owner->GetRenderer()->GetScreenInfo().height);

    SORE_Kernel::InputTask& input = owner->GetInputTask();
    input.AddListener(SORE_Kernel::INPUT_ALL | SORE_Kernel::RESIZE,
                       std::bind1st(std::mem_fun(
                                        &SORE_GUI::TopWidget::PropagateEvents), top));
    input.AddListener(SORE_Kernel::RESIZE ,
                       std::bind1st(std::mem_fun(&gui::TopWidget::OnResize), top));
    debug = new DebugGUI(owner->GetRenderer(), owner->GetPool(),
                         owner->GetInputTask(), top);
    owner->GetRenderer()->AddGeometryProvider(top);

    SORE_Graphics::GLSLShaderPtr shader =
        owner->GetPool().GetResource<SORE_Graphics::GLSLShader>("data/Shaders/point_sprite.shad");
    SORE_Graphics::Texture2DPtr texture =
        owner->GetPool().GetResource<SORE_Graphics::Texture2D>("data/Textures/particle.tga");
    particles = new ParticleSystem(texture, shader);

    owner->GetRenderer()->AddGeometryProvider(particles);

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
    
    kiss_cfg = kiss_fftr_alloc(kFFTSamples, 0, 0, 0);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
    glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SPRITE);
}

void DefaultState::Frame(int elapsed)
{
    debug->Frame(elapsed);

    system->update();
}

void DefaultState::Quit()
{
    owner->PopState();
}

bool DefaultState::HandleEscapeKey(SORE_Kernel::Event* e)
{
    if(e->type == SORE_Kernel::KEYDOWN && e->key.keySym == SDLK_ESCAPE)
    {
        Quit();
        return true;
    }
    return false;
}

SORE_Graphics::camera_info DefaultState::GetCamera()
{
    SORE_Graphics::ProjectionInfo proj;
    proj.type = SORE_Graphics::ORTHO2D;
    proj.useScreenRatio = true;
    proj.useScreenCoords = false;
    proj.left = -1.0f;
    proj.right = 1.0f;
    
    SORE_Math::Matrix4<float> identity;
    SORE_Graphics::camera_info cam = {proj, identity};
    return cam;
}

void DefaultState::GotSamples(float* buffer, unsigned int length, int channels)
{
    int sample_rate;
    system->getSoftwareFormat(&sample_rate, 0, 0, 0, 0, 0);

    assert(channels == 2);
    assert(length == kFFTSamples * channels);
    // APP_LOG(SORE_Logging::LVL_INFO, boost::format("Got %d samples (%d channels)") % length % channels);

    kiss_fft_scalar timedata[kFFTSamples];
    kiss_fft_cpx    freqdata[kFFTSamples/2 + 1];

    //calculate mono fft
    for(int i = 0; i < kFFTSamples; ++i)
    {
        timedata[i] = (buffer[i*2] + buffer[i*2 + 1]) / 2.0f;
    }

    kiss_fftr(kiss_cfg, timedata, freqdata);

    for(int i = 0; i < kNumSpectrumWindows; ++i)
    {
        spectrum[i] = 0.0f;
    }

    // divide by two: only get resolution out of bottom half of fft
    const int samples = (kFFTSamples / 2);
    const int samples_per_window = samples / kNumSpectrumWindows;
    // APP_LOG(SORE_Logging::LVL_INFO, boost::format("Need to analyze %d samples of width %d Hz (sample rate = %d)") % samples % (sample_rate / samples) % sample_rate);
    for(int i = 0; i <= samples; ++i)
    {
        kiss_fft_scalar mag  = freqdata[i + 1].r * freqdata[i + 1].r + freqdata[i + 1].i * freqdata[i + 1].i;
        spectrum[i / samples_per_window] += mag;
        // APP_LOG(SORE_Logging::LVL_INFO, boost::format("Got sample: (%f, %f) magnitude %f") % freqdata[i].r % freqdata[i].i % mag);
    }

    for(int i = 0; i < kNumSpectrumWindows; ++i)
    {
        spectrum[i] /= samples_per_window;
    }
}