#ifdef _MSC_VER //boost warnings in MSVC++
#pragma warning( push )
#pragma warning( disable : 4180 )
#endif

#include "app_log.h"
#include "debug_gui.h"

#include <boost/bind.hpp>

#include <sore_font.h>
#include <sore_topwidget.h>
#include <sore_textwidget.h>

namespace gui = SORE_GUI;

DebugGUI::DebugGUI(SORE_Graphics::Renderer& r, SORE_Resource::ResourcePool &pool,
                   SORE_Kernel::InputTask& input, gui::Widget* top)
    : renderer(r), renderStats(0), version(0)
{
    boost::shared_ptr<SORE_Font::Font> guiFont =
        pool.GetResource<SORE_Font::Font>("data/ix_style/LiberationSans-Regular.ttf");

    renderStats = new gui::RenderStats(pool, renderer, top);

    version = new gui::TextWidget(gui::SVec(gui::SUnit(0.0, 0), gui::SUnit(1.0, -26)),
                                  *guiFont, 24, GetVersionString(),
                                  SORE_Graphics::White, top);
}

DebugGUI::~DebugGUI()
{
    delete version;
    delete renderStats;
}

void DebugGUI::SetVisible(bool visible)
{
    version->SetVisible(visible);
    renderStats->SetVisible(visible);
}

void DebugGUI::Frame(int elapsed)
{
    renderStats->Frame(elapsed);
}

#ifdef _MSC_VER //boost warnings in MSVC++
#pragma warning( pop )
#endif
