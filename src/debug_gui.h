#ifndef DEBUG_GUI
#define DEBUG_GUI

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
    DebugGUI(SORE_Graphics::Renderer& r, gui::Widget* parent);

private:
    gui::RenderStats* renderStats;
    gui::TextWidget* version;
};

#endif
