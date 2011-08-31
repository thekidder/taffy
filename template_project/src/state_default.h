#ifndef STATE_DEFAULT
#define STATE_DEFAULT

#include "debug_gui.h"

#include <sore_inputdistributor.h>
#include <sore_gamestate.h>
#include <sore_pipeline_renderer.h>
#include <sore_topwidget.h>

class DefaultState : public SORE_Game::Gamestate
{
public:
    DefaultState(SORE_Game::GamestateStack& stack);
    ~DefaultState();

    void Frame(int elapsed);
    void Render();
    const char* GetName() const {return "Default state";}

    virtual bool OnEvent(const SORE_Kernel::Event& e); 
private:
    bool HandleEscapeKey(const SORE_Kernel::Event& e);
    void Quit();
    
    SORE_Graphics::PipelineRenderer renderer;
    SORE_Kernel::InputDistributor distributor;

    SORE_GUI::TopWidget top;
    DebugGUI* debug;
};

#endif
