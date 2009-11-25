#include <boost/format.hpp>

#include "sore_framewindow.h"
#include "sore_textwidget.h"
#include "../sore_renderer_std.h"

#include "sore_renderstats.h"

namespace SORE_GUI
{
    RenderStats::RenderStats(SORE_Resource::ResourcePool& pool,
                             SORE_Graphics::Renderer *r,
                             SORE_GUI::Widget *parent) :
        renderer(r), frame(0), draws(0), drawsLabel(0), fps(0), fpsLabel(0),
        polys(0), polysLabel(0), ms(0), msLabel(0)
    {
        SORE_Font::Font* font =
            pool.GetResource<SORE_Font::Font>
            ("data/ix_style/LiberationSans-Regular.ttf");

        frame = new SORE_GUI::FrameWindow(SVec(SUnit(0.0, 200), SUnit(0.0, 180)),
                                          SVec(SUnit(0.0, 10), SUnit(0.0, 10)),
                                          "Renderer Stats", pool, parent);

        fpsLabel   = new SORE_GUI::TextWidget(SVec(SUnit(0.0, 5), SUnit(0.0, 0)),
                                              *font, 16, "FPS:",
                                              SORE_Graphics::White, frame);
        drawsLabel = new SORE_GUI::TextWidget(SVec(SUnit(0.0, 5), SUnit(0.0, 16)),
                                              *font, 16, "Draw Calls:",
                                              SORE_Graphics::White, frame);
        polysLabel = new SORE_GUI::TextWidget(SVec(SUnit(0.0, 5), SUnit(0.0, 32)),
                                              *font, 16, "Polygons:",
                                              SORE_Graphics::White, frame);
        msLabel    = new SORE_GUI::TextWidget(SVec(SUnit(0.0, 5), SUnit(0.0, 48)),
                                              *font, 16, "Milliseconds:",
                                              SORE_Graphics::White, frame);

        fps        = new SORE_GUI::TextWidget(SVec(SUnit(1.0, -60), SUnit(0.0, 0)),
                                              *font, 16, "0", SORE_Graphics::White,
                                              frame);
        draws      = new SORE_GUI::TextWidget(SVec(SUnit(1.0, -60), SUnit(0.0, 16)),
                                              *font, 16, "0", SORE_Graphics::White,
                                              frame);
        polys      = new SORE_GUI::TextWidget(SVec(SUnit(1.0, -60), SUnit(0.0, 32)),
                                              *font, 16, "0", SORE_Graphics::White,
                                              frame);
        ms         = new SORE_GUI::TextWidget(SVec(SUnit(1.0, -60), SUnit(0.0, 48)),
                                              *font, 16, "0", SORE_Graphics::White,
                                              frame);
    }

    RenderStats::~RenderStats()
    {
        delete frame;

        delete drawsLabel;
        delete fpsLabel;
        delete polysLabel;
        delete msLabel;

        delete draws;
        delete fps;
        delete polys;
        delete ms;
    }

    void RenderStats::SetVisible(bool visible)
    {
        frame->SetVisible(visible);
    }

    void RenderStats::Frame(int elapsed)
    {
        int rendererFPS = static_cast<int>(renderer->GetFPS());

        const int FPS_HIGH = 60;
        const int FPS_MID  = 30;

        if(rendererFPS > FPS_HIGH)
            fps->SetColor(SORE_Graphics::Green);
        else if(rendererFPS > FPS_MID)
            fps->SetColor(SORE_Graphics::Yellow);
        else
            fps->SetColor(SORE_Graphics::Red);

        fps->UpdateText(boost::str(boost::format("%d") % rendererFPS));
        draws->UpdateText(boost::str(boost::format("%d") % renderer->GetDrawCalls()));
        polys->UpdateText(boost::str(boost::format("%d") % renderer->GetNumPolys()));
        ms->UpdateText(boost::str(boost::format("%d") % renderer->GetFrameMS()));
    }
}
