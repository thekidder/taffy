#include <boost/format.hpp>

#include <sore_framewindow.h>
#include <sore_textwidget.h>
#include <sore_renderer.h>

#include <sore_renderstats.h>

namespace SORE_GUI
{
    RenderStats::RenderStats(SORE_Graphics::IRenderer* r,
                             SORE_GUI::Widget *parent)
    : FrameWindow(SVec(SUnit(0.0, 200), SUnit(0.0, 180)),
                  SVec(SUnit(0.0, 10), SUnit(0.0, 10)),
                  "Renderer Stats", parent),
    renderer(r), draws(0), drawsLabel(0), fps(0), fpsLabel(0),
    polys(0), polysLabel(0), ms(0), msLabel(0)
    {
        SORE_Resource::FontPtr font;

        fpsLabel   = new SORE_GUI::TextWidget(SVec(SUnit(0.0, 5), SUnit(0.0, 0)),
                                              font, 16, "FPS:",
                                              SORE_Graphics::White, this);
        drawsLabel = new SORE_GUI::TextWidget(SVec(SUnit(0.0, 5), SUnit(0.0, 16)),
                                              font, 16, "Draw Calls:",
                                              SORE_Graphics::White, this);
        polysLabel = new SORE_GUI::TextWidget(SVec(SUnit(0.0, 5), SUnit(0.0, 32)),
                                              font, 16, "Polygons:",
                                              SORE_Graphics::White, this);
        msLabel    = new SORE_GUI::TextWidget(SVec(SUnit(0.0, 5), SUnit(0.0, 48)),
                                              font, 16, "Milliseconds:",
                                              SORE_Graphics::White, this);

        fps        = new SORE_GUI::TextWidget(SVec(SUnit(1.0, -60), SUnit(0.0, 0)),
                                              font, 16, "0", SORE_Graphics::White,
                                              this);
        draws      = new SORE_GUI::TextWidget(SVec(SUnit(1.0, -60), SUnit(0.0, 16)),
                                              font, 16, "0", SORE_Graphics::White,
                                              this);
        polys      = new SORE_GUI::TextWidget(SVec(SUnit(1.0, -60), SUnit(0.0, 32)),
                                              font, 16, "0", SORE_Graphics::White,
                                              this);
        ms         = new SORE_GUI::TextWidget(SVec(SUnit(1.0, -60), SUnit(0.0, 48)),
                                              font, 16, "0", SORE_Graphics::White,
                                              this);
    }

    void RenderStats::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
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

        fps->SetPosition(SVec(SUnit(1.0, -10 - fps->GetSize(HORIZONTAL)),
                              SUnit(0.0, 0)));
        draws->SetPosition(SVec(SUnit(1.0, -10 - draws->GetSize(HORIZONTAL)),
                                SUnit(0.0, 16)));
        polys->SetPosition(SVec(SUnit(1.0, -10 - polys->GetSize(HORIZONTAL)),
                                SUnit(0.0, 32)));
        ms->SetPosition(SVec(SUnit(1.0, -10 - ms->GetSize(HORIZONTAL)),
                             SUnit(0.0, 48)));

        RenderFrame(imm_mode);
    }
}
