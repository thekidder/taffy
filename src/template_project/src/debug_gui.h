#ifndef DEBUG_GUI
#define DEBUG_GUI

#include <sore_profilerstats.h>
#include <sore_renderer.h>
#include <sore_renderstats.h>

namespace SORE_GUI
{
    class TopWidget;
    class TextWidget;
}
namespace gui = SORE_GUI;

class DebugGUI : public SORE_GUI::Widget
{
public:
    DebugGUI(SORE_Graphics::Renderer& r, SORE_Profiler::Profiler* p, gui::Widget* parent);

private:
    gui::RenderStats* renderStats;
    gui::ProfilerStats* profilerStats;
    gui::TextWidget* version;
};

#endif
