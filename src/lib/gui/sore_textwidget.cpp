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
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
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

#include <sore_textwidget.h>

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
