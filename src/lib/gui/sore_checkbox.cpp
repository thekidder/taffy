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

#include <sore_checkbox.h>
#include <sore_glslshader_loader.h>
#include <sore_texture2d_loader.h>

SORE_GUI::Checkbox::Checkbox(SUnit size, SVec position, Widget* parent)
    : Widget(SVec(size, size), position, parent), is_checked(false)
{
    shader = shaderCache.Get(Style()["Checkbox"]["shader"].asString());
    normal = textureCache.Get(Style()["Checkbox"]["texture"].asString());
    checked = textureCache.Get(Style()["Checkbox"]["texture_checked"].asString());

    texcoords = SORE_Math::Rect<float>(
        static_cast<float>(Style()["Checkbox"]["tex_coords"][0u].asDouble()),
        static_cast<float>(Style()["Checkbox"]["tex_coords"][1u].asDouble()),
        static_cast<float>(Style()["Checkbox"]["tex_coords"][2u].asDouble()),
        static_cast<float>(Style()["Checkbox"]["tex_coords"][3u].asDouble()));

    texcoords_checked = SORE_Math::Rect<float>(
        static_cast<float>(Style()["Checkbox"]["tex_coords_checked"][0u].asDouble()),
        static_cast<float>(Style()["Checkbox"]["tex_coords_checked"][1u].asDouble()),
        static_cast<float>(Style()["Checkbox"]["tex_coords_checked"][2u].asDouble()),
        static_cast<float>(Style()["Checkbox"]["tex_coords_checked"][3u].asDouble()));

}

void SORE_GUI::Checkbox::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
{
    imm_mode.SetShader(shader);
    imm_mode.SetColor(SORE_Graphics::White);
    imm_mode.SetBlendMode(SORE_Graphics::BLEND_SUBTRACTIVE);
    imm_mode.SetTransform(
        SORE_Graphics::TransformationPtr(
                new SORE_Math::Matrix4<float>(
                    GetPositionMatrix())));

    SORE_Math::Rect<float> coords;
    if(!is_checked)
    {
        imm_mode.SetTexture(normal);
        coords = texcoords;
    }
    else
    {
        imm_mode.SetTexture(checked);
        coords = texcoords_checked;
    }

    float width  = static_cast<float>(GetSize(HORIZONTAL));
    float height = static_cast<float>(GetSize(VERTICAL));
    
    imm_mode.DrawQuad(
        0.0f,  0.0f,   0.0f,
        0.0f,  height, 0.0f,
        width, 0.0f,   0.0f,
        width, height, 0.0f,
        coords);
}

bool SORE_GUI::Checkbox::ProcessEvents(const SORE_Kernel::Event& e)
{
    switch(e.type)
    {
    case SORE_Kernel::MOUSEBUTTONUP:
        if(HasFocus())
        {
            is_checked = !is_checked;
            return true;
        }
        break;
    case SORE_Kernel::MOUSEBUTTONDOWN:
        return true;
    case SORE_Kernel::MOUSELEAVE:
        ClearFocus();
        return true;
    default:
        break;
    }
    return false;
}