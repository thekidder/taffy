#include <boost/bind.hpp>

#include <sore_gamestate_manager.h>

#include "app_log.h"
#include "state_default.h"

#include <fmod.hpp>

const int kFFTSamples = 512;

DefaultState::DefaultState() : top(0), debug(0)
{
}

DefaultState::~DefaultState()
{
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

    SORE_Graphics::camera_callback guiCam = boost::bind(
        &SORE_GUI::TopWidget::GetCamera,
        boost::ref(top));

    SORE_Graphics::camera_callback_table cameras;
    cameras["gui"] = guiCam;
    cameras["normal"] = guiCam;
    owner->GetRenderer()->SetCameraTable(cameras);

    FMOD::System_Create(&system);
    system->init(100, FMOD_INIT_NORMAL, 0);
    system->createStream("music.mp3", FMOD_SOFTWARE, 0, &sound);
    system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
    
    kiss_cfg = kiss_fftr_alloc(kFFTSamples, 0, 0, 0);
}

void DefaultState::Frame(int elapsed)
{
    debug->Frame(elapsed);

    system->update();

    /*kiss_fft_scalar timedata[kFFTSamples];
    kiss_fft_cpx    freqdata[kFFTSamples/2 + 1];

    sound.read(timedata, kFFTSamples);

    kiss_fftr(kiss_cfg, timedata, freqdata);

    for(int i = 0; i <= kFFTSamples/2; ++i)
    {
        kiss_fft_scalar mag = freqdata[i].r * freqdata[i].r + freqdata[i].i * freqdata[i].i;
        APP_LOG(SORE_Logging::LVL_INFO, boost::format("Got sample: (%f, %f) magnitude %f") % freqdata[i].r % freqdata[i].i % mag);
    }*/
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

