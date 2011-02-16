#ifndef DEBUG_GUI
#define DEBUG_GUI

#include <sore_input.h>
#include <sore_renderer_std.h>
#include <sore_resource.h>
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
    DebugGUI(SORE_Graphics::Renderer& r, SORE_Resource::ResourcePool& pool,
             SORE_Kernel::InputTask& input, gui::Widget* top);
    ~DebugGUI();

    void SetVisible(bool visible = true);

    void Frame(int elapsed);
private:
    SORE_Graphics::Renderer& renderer;
    gui::RenderStats* renderStats;
    gui::TextWidget* version;
};

#endif
