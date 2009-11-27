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

#include "../sore_logger.h"
#include "sore_topwidget.h"

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

    SORE_Graphics::ProjectionInfo TopWidget::GetProjection(SORE_Graphics::ScreenInfo s)
    {
        if(s.width != GetSize().GetHorizontal().GetAbsolute() ||
             s.height != GetSize().GetVertical().GetAbsolute())
        {
            ENGINE_LOG(SORE_Logging::LVL_WARNING,
                       "ScreenInfo dimensions do not match that of the "
                       "top-level container");
            UpdateResolution(s.width, s.height);
        }
        SORE_Graphics::ProjectionInfo proj;
        proj.type = SORE_Graphics::ORTHO2D;
        proj.left = 0;
        proj.right = static_cast<float>(s.width);
        proj.top = static_cast<float>(s.height);
        proj.bottom = 0;
        proj.useScreenRatio = false;
        return proj;
    }

    /*SORE_Graphics::render_list TopWidget::GetThisRenderList()
    {
        SORE_Graphics::render_list list;
        return list;
    }
    */

    bool TopWidget::ProcessEvents(SORE_Kernel::Event* e)
    {
        return false;
    }
}
