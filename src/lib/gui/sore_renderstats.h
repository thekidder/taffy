#ifndef RENDERSTATS_H
#define RENDERSTATS_H

#include <sore_dll.h>
#include <sore_framewindow.h>

namespace SORE_Graphics
{
    class Renderer;
}

namespace SORE_GUI
{
    class TextWidget;

    class SORE_EXPORT RenderStats : public FrameWindow
    {
    public:
        RenderStats(SORE_Graphics::IRenderer* r,
                    SORE_GUI::Widget* parent = 0);
    private:
        virtual void UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode);

        SORE_Graphics::IRenderer* renderer;

        SORE_GUI::TextWidget* draws, *drawsLabel;
        SORE_GUI::TextWidget* fps, *fpsLabel;
        SORE_GUI::TextWidget* polys, *polysLabel;
        SORE_GUI::TextWidget* ms, *msLabel;
    };
}

#endif
