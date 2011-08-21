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
                             Widget *parent)
    : FrameWindow(SVec(SUnit(200), SUnit(180)),
                  SVec(SUnit(10), SUnit(10)),
                  "Renderer Stats", parent),
    renderer(r), draws(0), drawsLabel(0), fps(0), fpsLabel(0),
    polys(0), polysLabel(0), ms(0), msLabel(0)
    {
        int textHeight = static_cast<int>(Style()["RenderStats"]["text_size"].asInt());
        SUnit height(0.0, textHeight);
        fpsLabel   = new TextWidget(height, SVec(SUnit(5), SUnit(0)), this, "FPS:");
        drawsLabel = new TextWidget(height, SVec(SUnit(5), SUnit(textHeight)), this, "Draw Calls:"),
        polysLabel = new TextWidget(height, SVec(SUnit(5), SUnit(textHeight*2)), this, "Polygons:"),
        msLabel    = new TextWidget(height, SVec(SUnit(5), SUnit(textHeight*3)), this, "Milliseconds"),

        fps        = new TextWidget(height, SVec(SUnit(1.0, -60), SUnit(0)), this);
        draws      = new TextWidget(height, SVec(SUnit(1.0, -60), SUnit(textHeight)), this);
        polys      = new TextWidget(height, SVec(SUnit(1.0, -60), SUnit(textHeight*2)), this);
        ms         = new TextWidget(height, SVec(SUnit(1.0, -60), SUnit(textHeight*3)), this);
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

        int textHeight = static_cast<int>(Style()["RenderStats"]["text_size"].asInt());
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
