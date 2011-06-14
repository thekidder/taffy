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

namespace SORE_GUI
{
    FrameWindow::FrameWindow(SVec s, SVec p, const std::string& t,
                             Widget* parent_)
        : FrameWidget(s, p, SCALE_CENTER, parent_), title(t), titleText(0), dragged(false)
    {
        std::string styleDir("data/");
        styleDir += GetStyle() + "/";

        titleText = new TextWidget(SVec(SUnit(0.0, 0), SUnit(0.0, 0)), fontCache.Get("LiberationSans-Regular.ttf"), 24, t, SORE_Graphics::White, this);

        SetBorderSizes(32.0f, 32.0f, 32.0f, 32.0f);
        UpdatePosition();
    }

    void FrameWindow::UpdatePosition()
    {
        float left = static_cast<float>(
            (GetSize(HORIZONTAL) - titleText->GetSize(HORIZONTAL)) / 2);
        float up = 2.0f;
        titleText->SetPosition(SVec(SUnit(0.0, static_cast<int>(left-4)), 
                                    SUnit(0.0, static_cast<int>(up-32))));
    }

    unsigned int FrameWindow::GetClientSize(unit_type type) const
    {
        if(type == HORIZONTAL)
            return GetSize(type) - 8;
        else
            return GetSize(type) - 36;
    }

    unsigned int FrameWindow::GetClientPosition(unit_type type) const
    {
        if(type == HORIZONTAL)
            return 4;
        else
            return 32;
    }

    FrameWindow::~FrameWindow()
    {
        delete titleText;
    }

    bool FrameWindow::ProcessEvents(const SORE_Kernel::Event& e)
    {
        if(e.type == SORE_Kernel::MOUSEBUTTONDOWN)
        {
            dragged = true;
            return true;
        }
        else if(e.type == SORE_Kernel::MOUSEBUTTONUP)
        {
            dragged = false;
            return true;
        }
        else if(e.type == SORE_Kernel::MOUSEMOVE)
        {
            if(dragged)
            {
                SetPosition(SVec(SUnit(GetPosition().GetHorizontal().GetRelative(),
                                       GetPosition().GetHorizontal().GetAbsolute() +
                                       e.mouse.xmove),
                                 SUnit(GetPosition().GetVertical().GetRelative(),
                                       GetPosition().GetVertical().GetAbsolute() +
                                       e.mouse.ymove)));
                return true;
            }
        }
        return false;
    }

    void FrameWindow::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
    {
        RenderFrame(imm_mode);
    }
}
