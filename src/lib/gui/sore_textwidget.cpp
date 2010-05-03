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

#include "sore_textwidget.h"

namespace SORE_GUI
{
    TextWidget::TextWidget(
        SVec p, SORE_Font::Font& f, unsigned int h, const std::string& t,
        const SORE_Graphics::Color& c, Widget* parent)
        : Widget(SVec(SUnit(), SUnit()), p, parent), face(f), height(h),
          text(f, h, t, c), color(c)
    {
        UpdateText(t);
    }

    void TextWidget::UpdateText(const std::string& t)
    {
        text.UpdateText(t, color);
        SetSize(SVec(SUnit(0.0, text.GetWidth()), SUnit(0.0, text.GetHeight())));
        UpdateCache();
    }

    void TextWidget::SetColor(const SORE_Graphics::Color& c)
    {
        color = c;
        text.UpdateText(text.GetText(), color);
        UpdateCache();
    }

    void TextWidget::UpdatePosition()
    {
        UpdateCache();
    }

    void TextWidget::UpdateCache()
    {
        all.clear();
        std::vector<SORE_Graphics::Renderable> raw = text.GetGeometry();

        std::vector<SORE_Graphics::Renderable>::iterator it;
        for(it = raw.begin(); it != raw.end(); ++it)
        {
            SORE_Graphics::Renderable r(*it);
            SORE_Graphics::TransformationPtr m(
                new SORE_Math::Matrix4<float>(
                    *it->GetTransform() * GetPositionMatrix()));
            r.SetTransform(m);
            r.SetLayer(GUI_LAYER);
            all.push_back(r);
        }
    }

    std::vector<SORE_Graphics::Renderable> TextWidget::GetThisRenderList()
    {
        return all;
    }

    bool TextWidget::ProcessEvents(SORE_Kernel::Event* e)
    {
        return false;
    }

    void TextWidget::OnGLReload()
    {
        text.UpdateText(text.GetText(), color);
        UpdateCache();
    }
}
