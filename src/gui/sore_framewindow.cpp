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

#include "sore_framewindow.h"

namespace SORE_GUI
{
    FrameWindow::FrameWindow(SVec s, SVec p, const std::string& t,
                             SORE_Resource::ResourcePool& pool, Widget* par)
        : FrameWidget(s, p, SCALE_CENTER, par), title(t), titleText(0), dragged(false)
    {
        std::string styleDir("data/");
        styleDir += GetStyle() + "/";

        SORE_Graphics::Texture2DPtr tex =
            pool.GetResource<SORE_Graphics::Texture2D>(styleDir + "frame.tga");
        SORE_Graphics::GLSLShaderPtr shad =
            pool.GetResource<SORE_Graphics::GLSLShader>("data/Shaders/default.shad");
        SetBorderSizes(32.0f, 32.0f, 32.0f, 32.0f);
        SetTexture(tex);
        SetShader(shad);

        font = pool.GetResource<SORE_Font::Font>(
            styleDir + "LiberationSans-Regular.ttf");
        titleText = new SORE_Graphics::Text(*font, 24, title);

        UpdatePosition();
    }

    void FrameWindow::UpdatePosition()
    {
        float left = static_cast<float>((GetSize(HORIZONTAL) -
                                         titleText->GetWidth()) / 2);
        float up = 2.0f;
        titleText->SetTransform
            (GetPositionMatrix() *
             SORE_Math::Matrix4<float>::GetTranslation(left, up, 0.0f));
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

    bool FrameWindow::ProcessEvents(SORE_Kernel::Event* e)
    {
        if(e->type == SORE_Kernel::MOUSEBUTTONDOWN)
        {
            dragged = true;
            return true;
        }
        else if(e->type == SORE_Kernel::MOUSEBUTTONUP)
        {
            dragged = false;
            return true;
        }
        else if(e->type == SORE_Kernel::MOUSEMOVE)
        {
            if(dragged)
            {
                SetPosition(SVec(SUnit(GetPosition().GetHorizontal().GetRelative(),
                                       GetPosition().GetHorizontal().GetAbsolute() +
                                       e->mouse.xmove),
                                 SUnit(GetPosition().GetVertical().GetRelative(),
                                       GetPosition().GetVertical().GetAbsolute() +
                                       e->mouse.ymove)));
                return true;
            }
        }
        return false;
    }

    std::vector<SORE_Graphics::Renderable> FrameWindow::GetThisRenderList()
    {
        std::vector<SORE_Graphics::Renderable> list;

        std::vector<SORE_Graphics::Renderable> frame = GetChunks();
        std::copy(frame.begin(), frame.end(), std::back_inserter(list));
        std::vector<SORE_Graphics::Renderable> text = titleText->GetGeometry();
        std::copy(text.begin(), text.end(), std::back_inserter(list));

        return list;
    }
}
