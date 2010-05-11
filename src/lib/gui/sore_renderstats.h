#ifndef RENDERSTATS_H
#define RENDERSTATS_H

#include <sore_dll.h>
#include <sore_resource.h>

namespace SORE_Graphics
{
    class Renderer;
}

namespace SORE_GUI
{
    class Widget;
    class FrameWindow;
    class TextWidget;

    class SORE_EXPORT RenderStats
    {
    public:
        RenderStats(SORE_Resource::ResourcePool& pool, SORE_Graphics::Renderer& r,
                       SORE_GUI::Widget* parent = 0);
        ~RenderStats();

        void SetVisible(bool visible = true);

        void Frame(int elapsed);
    private:
        SORE_Graphics::Renderer& renderer;

        SORE_GUI::FrameWindow* frame;
        SORE_GUI::TextWidget* draws, *drawsLabel;
        SORE_GUI::TextWidget* fps, *fpsLabel;
        SORE_GUI::TextWidget* polys, *polysLabel;
        SORE_GUI::TextWidget* ms, *msLabel;
    };
}

#endif
