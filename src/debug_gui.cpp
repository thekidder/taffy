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

using gui::SVec;
using gui::SUnit;

DebugGUI::DebugGUI(
    SORE_Graphics::Renderer& r,
    SORE_Profiler::Profiler* p,
    gui::Widget* parent)
    : Widget(SVec(SUnit(1.0, 0), SUnit(1.0, 0)), SVec(SUnit(), SUnit()), parent),
    renderStats(0), profilerStats(0), version(0)
{
    renderStats = new gui::RenderStats(r, this);
    if(p)
    {
        profilerStats = new gui::ProfilerStats(gui::SVec(600, 300), gui::SVec(10, 220), *p, this);
        profilerStats->SetPosition(gui::SVec(gui::SUnit(), gui::SUnit(1.0, -profilerStats->GetSize(gui::VERTICAL) - 10)));
    }
    version = new gui::TextWidget(gui::SUnit(24), gui::SVec(gui::SUnit(1.0), gui::SUnit()), this, GetVersionString());
    version->SetPosition(gui::SVec(gui::SUnit(1.0, -version->GetSize(gui::HORIZONTAL)), gui::SUnit()));
}

#ifdef _MSC_VER //boost warnings in MSVC++
#pragma warning( pop )
#endif
