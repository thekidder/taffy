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

class DebugGUI
{
public:
    DebugGUI(SORE_Graphics::Renderer& r, gui::Widget* top);
    ~DebugGUI();

    void SetVisible(bool visible = true);

    void Frame(int elapsed);
private:
    gui::RenderStats* renderStats;
    gui::TextWidget* version;
};

#endif
