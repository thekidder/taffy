// $Id$

#ifndef STATE_DEFAULT
#define STATE_DEFAULT

#include <sore_gamestate.h>
#include <sore_input.h>

class DefaultState : public SORE_Game::Gamestate
{
public:
    DefaultState();
    ~DefaultState();

    void Frame(int elapsed);
    const char* GetName() const {return "Default state";}
private:
    bool HandleEscapeKey(SORE_Kernel::Event* e);
    void Init();
    void Quit();
};

#endif
