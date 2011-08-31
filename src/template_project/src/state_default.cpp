#include "state_default.h"

#include <sore_gamestate_stack.h>
#include <sore_pipe.h>
#include <sore_sample.h>

#include <boost/bind.hpp>

DefaultState::DefaultState(SORE_Game::GamestateStack& stack) 
    : Gamestate(stack), 
    renderer(gamestateStack.Profiler()),
    top(gamestateStack.FontCache(),
        gamestateStack.ShaderCache(),
        gamestateStack.TextureCache(),
        gamestateStack.Profiler()),
    debug(0)
{
    gamestateStack.PackageCache().AddPackage("ix_style.sdp");
    gamestateStack.PackageCache().AddPackage("default_resources.sdp");

    // gui gets to look at all events first
    distributor.AddListener(
        SORE_Kernel::INPUT_ALL | SORE_Kernel::RESIZE,
        boost::bind(&SORE_GUI::TopWidget::PropagateEvents, boost::ref(top), _1));

    // renderer needs to know about screen size
    distributor.AddListener(
        SORE_Kernel::RESIZE,
        boost::bind(&SORE_Graphics::PipelineRenderer::OnResize, boost::ref(renderer), _1));

    distributor.AddListener(
        SORE_Kernel::KEYDOWN,
         boost::bind(&DefaultState::HandleEscapeKey, this, _1));

    // setup the style
    top.SetStyleName("ix");
    SORE_FileIO::InFile ix_style("data/ix.json", &stack.PackageCache());
    top.LoadStyle(ix_style);

    debug = new DebugGUI(renderer, gamestateStack.Profiler(), &top);

    renderer.AddGeometryProvider(top.GetGeometryProvider());

    SORE_Graphics::camera_callback guiCam = boost::bind(
        &SORE_GUI::TopWidget::GetCamera,
        boost::ref(top));

    SORE_Graphics::camera_callback_table cameras;
    cameras["gui"] = guiCam;
    renderer.SetCameraTable(cameras);

    // set up render pipeline
    SORE_Graphics::Pipe* sorter = new SORE_Graphics::SortingPipe(SORE_Graphics::renderableSort, gamestateStack.Profiler());

    SORE_Graphics::Pipe* guiPipe = new SORE_Graphics::FilterPipe(SORE_Graphics::KeywordFilter("gui"), gamestateStack.Profiler());
    guiPipe->AddChildPipe(new SORE_Graphics::RenderPipe("gui"));

    renderer.RootPipe()->AddChildPipe(sorter);
    sorter->AddChildPipe(guiPipe);
}

DefaultState::~DefaultState()
{

}

bool DefaultState::OnEvent(const SORE_Kernel::Event& e)
{
    return distributor.DistributeEvent(e);
}

void DefaultState::Frame(int elapsed)
{
    PROFILE_BLOCK("All Logic", gamestateStack.Profiler());
    // draw gui
    top.Frame(elapsed);
}

void DefaultState::Render()
{
    renderer.Render();
}

void DefaultState::Quit()
{
    gamestateStack.PopState();
}

bool DefaultState::HandleEscapeKey(const SORE_Kernel::Event& e)
{
    if(e.type == SORE_Kernel::KEYDOWN && e.key.keySym == SORE_Kernel::Key::SSYM_ESCAPE)
    {
        Quit();
        return true;
    }
    return false;
}

