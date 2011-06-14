/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#include <sore_font_loader.h>
#include <sore_framewindow.h>
#include <sore_renderer.h>
#include <sore_renderstats.h>
#include <sore_textwidget.h>

#include <boost/format.hpp>

namespace SORE_GUI
{
    RenderStats::RenderStats(SORE_Graphics::Renderer& r,
                             SORE_GUI::Widget *parent)
    : FrameWindow(SVec(SUnit(0.0, 200), SUnit(0.0, 180)),
                  SVec(SUnit(0.0, 10), SUnit(0.0, 10)),
                  "Renderer Stats", parent),
    renderer(r), draws(0), drawsLabel(0), fps(0), fpsLabel(0),
    polys(0), polysLabel(0), ms(0), msLabel(0)
    {
        SORE_Resource::FontPtr font = fontCache.Get(Style()["RenderStats"]["font"].asString());

        unsigned int textHeight = static_cast<unsigned int>(Style()["RenderStats"]["text_size"].asInt());
        SUnit height(0.0, textHeight);
        fpsLabel   = new SORE_GUI::TextWidget(height, SVec(SUnit(0.0, 5), SUnit(0.0, 0)),
                                              font, "FPS:",
                                              SORE_Graphics::White, this);
        drawsLabel = new SORE_GUI::TextWidget(height,SVec(SUnit(0.0, 5), SUnit(0.0, textHeight)),
                                              font, "Draw Calls:",
                                              SORE_Graphics::White, this);
        polysLabel = new SORE_GUI::TextWidget(height, SVec(SUnit(0.0, 5), SUnit(0.0, textHeight*2)),
                                              font, "Polygons:",
                                              SORE_Graphics::White, this);
        msLabel    = new SORE_GUI::TextWidget(height, SVec(SUnit(0.0, 5), SUnit(0.0, textHeight*3)),
                                              font, "Milliseconds:",
                                              SORE_Graphics::White, this);

        fps        = new SORE_GUI::TextWidget(height, SVec(SUnit(1.0, -60), SUnit(0.0, 0)),
                                              font, "0", SORE_Graphics::White,
                                              this);
        draws      = new SORE_GUI::TextWidget(height, SVec(SUnit(1.0, -60), SUnit(0.0, textHeight)),
                                              font, "0", SORE_Graphics::White,
                                              this);
        polys      = new SORE_GUI::TextWidget(height, SVec(SUnit(1.0, -60), SUnit(0.0, textHeight*2)),
                                              font, "0", SORE_Graphics::White,
                                              this);
        ms         = new SORE_GUI::TextWidget(height, SVec(SUnit(1.0, -60), SUnit(0.0, textHeight*3)),
                                              font, "0", SORE_Graphics::White,
                                              this);
    }

    void RenderStats::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
    {
        int rendererFPS = static_cast<int>(renderer.GetFPS());

        const int FPS_HIGH = 60;
        const int FPS_MID  = 30;

        if(rendererFPS > FPS_HIGH)
            fps->SetColor(SORE_Graphics::Green);
        else if(rendererFPS > FPS_MID)
            fps->SetColor(SORE_Graphics::Yellow);
        else
            fps->SetColor(SORE_Graphics::Red);

        fps->SetText(boost::str(boost::format("%d") % rendererFPS));
        draws->SetText(boost::str(boost::format("%d") % renderer.GetDrawCalls()));
        polys->SetText(boost::str(boost::format("%d") % renderer.GetNumPolys()));
        ms->SetText(boost::str(boost::format("%d") % renderer.GetFrameMS()));

        unsigned int textHeight = static_cast<unsigned int>(Style()["RenderStats"]["text_size"].asInt());
        fps->SetPosition(SVec(SUnit(1.0, -10 - fps->GetSize(HORIZONTAL)),
                              SUnit(0.0, 0)));
        draws->SetPosition(SVec(SUnit(1.0, -10 - draws->GetSize(HORIZONTAL)),
                                SUnit(0.0, textHeight)));
        polys->SetPosition(SVec(SUnit(1.0, -10 - polys->GetSize(HORIZONTAL)),
                                SUnit(0.0, textHeight*2)));
        ms->SetPosition(SVec(SUnit(1.0, -10 - ms->GetSize(HORIZONTAL)),
                             SUnit(0.0, textHeight*3)));

        RenderFrame(imm_mode);
    }
}
