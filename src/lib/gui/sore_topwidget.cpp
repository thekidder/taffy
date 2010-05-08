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

#include <sore_logger.h>
#include <sore_topwidget.h>

namespace SORE_GUI
{
    TopWidget::TopWidget(unsigned int width, unsigned int height)
        : Widget(SVec(SUnit(0.0, width), SUnit(0.0, height)),
                                SVec(SUnit(0.0, 0), SUnit(0.0, 0)))
    {
    }

    bool TopWidget::OnResize(SORE_Kernel::Event* e)
    {
        if(e->type == SORE_Kernel::RESIZE)
        {
            UpdateResolution(e->resize.w, e->resize.h);
            return true;
        }
        return false;
    }

    void TopWidget::UpdateResolution(unsigned int w, unsigned int h)
    {
        SetSize(SVec(SUnit(0.0, w), SUnit(0.0, h)));
    }

    SORE_Graphics::camera_info TopWidget::GetCamera()
    {
        SORE_Graphics::ProjectionInfo proj;
        proj.type = SORE_Graphics::ORTHO2D;
        proj.useScreenRatio = false;
        proj.useScreenCoords = true;

        SORE_Math::Matrix4<float> identity;

        SORE_Graphics::camera_info cam = {proj, identity};
        return cam;
    }

    void TopWidget::MakeUpToDate()
    {
        renderables.clear();
        renderables = GetRenderList();
    }

    std::vector<SORE_Graphics::Renderable>::iterator TopWidget::GeometryBegin()
    {
        return renderables.begin();
    }

    std::vector<SORE_Graphics::Renderable>::iterator TopWidget::GeometryEnd()
    {
        return renderables.end();
    }

    std::vector<SORE_Graphics::Renderable> TopWidget::GetThisRenderList()
    {
        std::vector<SORE_Graphics::Renderable> list;
        return list;
    }

    bool TopWidget::ProcessEvents(SORE_Kernel::Event* e)
    {
        return false;
    }
}
