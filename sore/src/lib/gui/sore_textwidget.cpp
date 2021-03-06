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
#include <sore_material.h>
#include <sore_material_loader.h>
#include <sore_textwidget.h>

namespace SORE_GUI
{
    TextWidget::TextWidget(
        SUnit height, SVec p, Widget* parent, 
        const std::string& t,
        const SORE_Graphics::Color& c)
        : Widget(SVec(SUnit(), height), p, parent), color(c)
    {
        face = fontCache.Get(Style()["TextWidget"]["font"].asString());
        material = materialCache.Clone(Style()["TextWidget"]["material"].asString());

        SetText(t);
    }

    void TextWidget::SetText(const std::string& t)
    {
        text = t;
        unsigned int h = static_cast<unsigned int>(GetSize(VERTICAL));
        SetSize(SVec(SUnit(static_cast<int>(face->Width(h, text))), GetSize().GetVertical()));
    }

    void TextWidget::SetColor(const SORE_Graphics::Color& c)
    {
        color = c;
    }

    void TextWidget::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
    {
        unsigned int h = static_cast<unsigned int>(GetSize(VERTICAL));
        imm_mode.SetMaterial(material);
        imm_mode.SetTransform(SORE_Graphics::MatrixPtr(new SORE_Math::Matrix4<float>(GetPositionMatrix())));
        imm_mode.SetColor(color);
        imm_mode.DrawString(0.0f, 0.0f, 0.0f, face, h, text);
    }

    bool TextWidget::ProcessEvents(const SORE_Kernel::Event& e)
    {
        return false;
    }
}
