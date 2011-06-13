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

DebugGUI::DebugGUI(
    SORE_Graphics::Renderer& r,
    gui::Widget* top)
    : renderStats(0), version(0)
{

    renderStats = new gui::RenderStats(r, top);

    //version = new gui::TextWidget(gui::SVec(gui::SUnit(1.0, -guiFont->Width(24, GetVersionString())), gui::SUnit(0.0, 0)),
    //                              *guiFont, 24, GetVersionString(), 
    //                              SORE_Graphics::White, top);
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
}

#ifdef _MSC_VER //boost warnings in MSVC++
#pragma warning( pop )
#endif
