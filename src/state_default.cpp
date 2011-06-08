#include "app_log.h"
#include "fmod_spectrum.h"
#include "state_default.h"

#include <sore_gamestate_manager.h>

#include <boost/bind.hpp>
#include <fmod.hpp>

#include <cassert>

typedef std::pair<float,float> Float_range_t;
float mapToRange(float value, Float_range_t original, Float_range_t newRange);

const int kFFTSamples = 1024;
const int kNumChannels = 2;

const Float_range_t kDisplayRangeDBFMOD(-60.0f, 0.0f);
const Float_range_t kDisplayRangeDBKISS(-60.0f, 0.0f);
const Float_range_t kDisplayRangeScreen(-1.0f, 1.0f);

DefaultState::DefaultState() 
    : top(0), debug(0), buffer(kFFTSamples * kNumChannels, kNumChannels), fmod_adapter(buffer),
    use_kiss(false), display_channel(LEFT)
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

    int sample_rate;
    system->getSoftwareFormat(&sample_rate, 0, 0, 0, 0, 0);

    fmod_spectrum = new FMOD_Spectrum(kFFTSamples, sample_rate, system);
    kiss_spectrum = new KISS_Spectrum(kFFTSamples, sample_rate);

    fmod_g_spectrum = new GeometricSpectrum(*fmod_spectrum, 20);
    kiss_g_spectrum = new GeometricSpectrum(*kiss_spectrum, 20);
}

void DefaultState::Frame(int elapsed)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
    glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SPRITE);

    debug->Frame(elapsed);

    fmod_spectrum->Update();
    system->update();

    particles->ClearParticles();

    CompressedSpectrum* spectrum;
    Float_range_t transform_range;
    if(use_kiss)
    {
        spectrum = kiss_g_spectrum;
        transform_range = kDisplayRangeDBKISS;
    }
    else
    {
        spectrum = fmod_g_spectrum;
        transform_range = kDisplayRangeDBFMOD;
    }
    float width = 2.0f / (spectrum->NumBuckets() - 1);
    for(int i = 0; i < spectrum->NumBuckets() - 1; ++i)
	{
        float z;
        switch(display_channel)
        {
        case LEFT:
            z  = spectrum->Left(i);
            break;
        case RIGHT:
            z  = spectrum->Right(i);
            break;
        case MIX:
            z  = spectrum->Mix(i);
        }
        z = mapToRange(z, transform_range, kDisplayRangeScreen);
        particles->AddParticle(Particle(-1.0f + width*i + width/2.0f, z, 0.0f, 48.0f));
    }
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
        case SDLK_c:
            switch(display_channel)
            {
            case LEFT:
                display_channel = RIGHT;
                break;
            case RIGHT:
                display_channel = MIX;
                break;
            case MIX:
                display_channel = LEFT;
            }
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

float mapToRange(float value, Float_range_t original, Float_range_t newRange)
{
    // map to a percentage (0,1)
    float mag = (value - original.first) / (original.second - original.first);
    // map percentage to new range
    float transformed = newRange.first + (newRange.second - newRange.first) * mag;

    // clamp
    return std::min(newRange.second, std::max(transformed, newRange.first));
}