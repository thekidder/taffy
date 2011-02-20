#include <boost/bind.hpp>

#include <sore_gamestate_manager.h>

#include "state_default.h"

DefaultState::DefaultState() : top(0), debug(0)
{
}

DefaultState::~DefaultState()
{
    delete debug;
    delete top;
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

}

void DefaultState::Frame(int elapsed)
{
    debug->Frame(elapsed);
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

