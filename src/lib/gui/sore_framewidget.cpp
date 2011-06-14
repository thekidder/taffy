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

#include <sore_geometry.h>
#include <sore_glslshader_loader.h>
#include <sore_logger.h>
#include <sore_sprite.h>
#include <sore_framewidget.h>
#include <sore_texture2d_loader.h>

namespace SORE_GUI
{
    FrameWidget::FrameWidget(SVec s, SVec p, size_mode m, Widget* parent_)
        : Widget(s, p, parent_), mode(m), leftBorder(16),
          rightBorder(16), topBorder(16), bottomBorder(16)
    {
        texture = textureCache.Get("frame.tga");
        shader = shaderCache.Get("default.shad");
    }

    void FrameWidget::SetBorderSizes(float l, float r, float t, float b)
    {
        switch(mode)
        {
        case SCALE_CENTER:
            leftBorder = l;
            rightBorder = r;
            topBorder = t;
            bottomBorder = b;
            break;
        case SCALE_ALL:
        {
            float h = static_cast<float>(GetSize(HORIZONTAL));
            float v = static_cast<float>(GetSize(VERTICAL));
            float f = h < v ? h : v;
            leftBorder = l/64.0f * f;
            rightBorder = r/64.0f * f;
            topBorder = t/64.0f * f;
            bottomBorder = b/64.0f * f;
            break;
        }
        default:
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       "Unknown sizing mode selected for FrameWidget");
            break;
        };
    }

    void FrameWidget::SetTexture(SORE_Resource::Texture2DPtr tex)
    {
        texture = tex;
    }

    void FrameWidget::SetShader(SORE_Resource::GLSLShaderPtr shad)
    {
        shader = shad;
    }


    void FrameWidget::RenderFrame(SORE_Graphics::ImmediateModeProvider& imm_mode)
    {
        imm_mode.SetShader(shader);
        imm_mode.SetTexture(texture);
        imm_mode.SetColor(SORE_Graphics::White);
        imm_mode.SetBlendMode(SORE_Graphics::BLEND_SUBTRACTIVE);
        imm_mode.SetTransform(
            SORE_Graphics::TransformationPtr(
                    new SORE_Math::Matrix4<float>(
                        GetPositionMatrix())));

        float width  = static_cast<float>(GetSize(HORIZONTAL));
        float height = static_cast<float>(GetSize(VERTICAL));

        SORE_Math::Rect<float> bounds;
        SORE_Math::Rect<float> texCoords;

        if(leftBorder + rightBorder < width && bottomBorder + topBorder < height)
        {
            //center
            texCoords = SORE_Math::Rect<float>(0.25f, 0.75f, 0.25f, 0.75f);
            imm_mode.DrawQuad(
                leftBorder,         topBorder,             0.0f,
                leftBorder,         height - bottomBorder, 0.0f,
                width - leftBorder, topBorder,             0.0f,
                width - leftBorder, height - bottomBorder, 0.0f,
                texCoords);
        }

        if(leftBorder && bottomBorder + topBorder < height)
        {
            //left border
            texCoords = SORE_Math::Rect<float>(0.0f, 0.25f, 0.25f, 0.75f);
            imm_mode.DrawQuad(
                0.0f,       topBorder,             0.0f,
                0.0f,       height - bottomBorder, 0.0f,
                leftBorder, topBorder,             0.0f,
                leftBorder, height - bottomBorder, 0.0f,
                texCoords);
        }

        if(leftBorder && topBorder)
        {
            //top left corner
            texCoords = SORE_Math::Rect<float>(0.0f, 0.25f, 0.0f, 0.25f);
            imm_mode.DrawQuad(
                0.0f,       0.0f,      0.0f,
                0.0f,       topBorder, 0.0f,
                leftBorder, 0.0f,      0.0f,
                leftBorder, topBorder, 0.0f,
                texCoords);
        }

        if(leftBorder + rightBorder < width && topBorder)
        {
            //top border
            texCoords = SORE_Math::Rect<float>(0.25f, 0.75f, 0.0f, 0.25f);
            imm_mode.DrawQuad(
                leftBorder,          0.0f,      0.0f,
                leftBorder,          topBorder, 0.0f,
                width - rightBorder, 0.0f,      0.0f,
                width - rightBorder, topBorder, 0.0f,
                texCoords);
        }

        if(rightBorder && topBorder)
        {
            //top right corner
            texCoords = SORE_Math::Rect<float>(0.75f, 1.0f, 0.0f, 0.25f);
            imm_mode.DrawQuad(
                width - rightBorder, 0.0f,      0.0f,
                width - rightBorder, topBorder, 0.0f,
                width,               0.0f,      0.0f,
                width,               topBorder, 0.0f,
                texCoords);
        }

        if(rightBorder && topBorder + bottomBorder < height)
        {
            //right border
            texCoords = SORE_Math::Rect<float>(0.75f, 1.0f, 0.25f, 0.75f);
            imm_mode.DrawQuad(
                width - rightBorder, topBorder,             0.0f,
                width - rightBorder, height - bottomBorder, 0.0f,
                width,               topBorder,             0.0f,
                width,               height - bottomBorder, 0.0f,
                texCoords);
        }

        if(rightBorder && bottomBorder)
        {
            //bottom right corner
            texCoords = SORE_Math::Rect<float>(0.75f, 1.0f, 0.75f, 1.0f);
            imm_mode.DrawQuad(
                width - rightBorder, height - bottomBorder, 0.0f,
                width - rightBorder, height,                0.0f,
                width,               height - bottomBorder, 0.0f,
                width,               height,                0.0f,
                texCoords);
        }

        if(bottomBorder && rightBorder + leftBorder < width)
        {
            //bottom border
            texCoords = SORE_Math::Rect<float>(0.25f, 0.75f, 0.75f, 1.0f);
            imm_mode.DrawQuad(
                leftBorder,          height - bottomBorder, 0.0f,
                leftBorder,          height,                0.0f,
                width - rightBorder, height - bottomBorder, 0.0f,
                width - rightBorder, height,                0.0f,
                texCoords);
        }

        if(leftBorder && bottomBorder)
        {
            //bottom left corner
            texCoords = SORE_Math::Rect<float>(0.0f, 0.25f, 0.75f, 1.0f);
            imm_mode.DrawQuad(
                0.0f,       height - bottomBorder, 0.0f,
                0.0f,       height,                0.0f,
                leftBorder, height - bottomBorder, 0.0f,
                leftBorder, height,                0.0f,
                texCoords);
        }
    }
}
