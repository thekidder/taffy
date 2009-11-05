#include <boost/bind.hpp>

#include <sore_gamestate_manager.h>

#include "state_default.h"

DefaultState::DefaultState()
{
}

DefaultState::~DefaultState()
{
}

void DefaultState::Init()
{
    owner->GetInputTask()->AddListener(SORE_Kernel::KEYDOWN,
                                       boost::bind(&DefaultState::HandleEscapeKey, this, _1));
}

void DefaultState::Frame(int elapsed)
{
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

