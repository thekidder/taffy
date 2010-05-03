/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
