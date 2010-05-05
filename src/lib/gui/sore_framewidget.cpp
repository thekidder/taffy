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

#include <sore_geometry.h>
#include <sore_sprite.h>
#include <sore_framewidget.h>

namespace SORE_GUI
{
    FrameWidget::FrameWidget(SVec s, SVec p, size_mode m, Widget* par)
        : Widget(s, p, par), mode(m), leftBorder(16),
          rightBorder(16), topBorder(16), bottomBorder(16)
    {
    }

    void FrameWidget::SetBorderSizes(float l, float r, float t, float b)
    {
        switch(mode)
        {
        case SCALE_CENTER:
            leftBorder = static_cast<unsigned int>(l);
            rightBorder = static_cast<unsigned int>(r);
            topBorder = static_cast<unsigned int>(t);
            bottomBorder = static_cast<unsigned int>(b);
            break;
        case SCALE_ALL:
        {
            float h = static_cast<float>(GetSize(HORIZONTAL));
            float v = static_cast<float>(GetSize(VERTICAL));
            float f = h < v ? h : v;
            leftBorder = static_cast<unsigned int>(l/64.0f * f);
            rightBorder = static_cast<unsigned int>(r/64.0f * f);
            topBorder = static_cast<unsigned int>(t/64.0f * f);
            bottomBorder = static_cast<unsigned int>(b/64.0f * f);
            break;
        }
        default:
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       "Unknown sizing mode selected for FrameWidget");
            break;
        };

        BuildGeometry();
    }

    void FrameWidget::SetTexture(SORE_Graphics::Texture2DPtr tex)
    {
        texture = tex;
        for(unsigned int i=0;i<9;++i)
        {
            chunk[i].AddTexture("texture", tex);
        }
    }

    void FrameWidget::SetShader(SORE_Graphics::GLSLShaderPtr shad)
    {
        shader = shad;
        for(unsigned int i=0;i<9;++i)
        {
            chunk[i].SetShader(shad);
        }
    }


    std::vector<SORE_Graphics::Renderable> FrameWidget::GetChunks() const
    {
        std::vector<SORE_Graphics::Renderable> all;
        for(unsigned int i=0;i<9;++i)
        {
            if(chunk[i].GetTransform())
                *chunk[i].GetTransform() = GetPositionMatrix();
            if(chunk[i].GetGeometryChunk())
                all.push_back(chunk[i]);
        }
        return all;
    }

    void FrameWidget::BuildGeometry()
    {
        unsigned int width = GetSize(HORIZONTAL);
        unsigned int height = GetSize(VERTICAL);

        SORE_Math::Rect<float> bounds;
        SORE_Math::Rect<float> texCoords;

        if(leftBorder + rightBorder < width && bottomBorder + topBorder < height)
        {
            //center
            bounds = SORE_Math::Rect<float>(
                static_cast<float>(leftBorder),
                static_cast<float>(width - leftBorder),
                static_cast<float>(topBorder),
                static_cast<float>(height - bottomBorder));
            texCoords = SORE_Math::Rect<float>(0.25f, 0.75f, 0.25f, 0.75f);

            chunk[0] = SORE_Graphics::MakeSprite(
                bounds, texCoords, 0.0f, texture, shader,
                GUI_LAYER, SORE_Graphics::BLEND_SUBTRACTIVE);
        }

        if(leftBorder && bottomBorder + topBorder < height)
        {
            //left border
            bounds = SORE_Math::Rect<float>(
                0.0f,
                static_cast<float>(leftBorder),
                static_cast<float>(topBorder),
                static_cast<float>(height - bottomBorder));
            texCoords = SORE_Math::Rect<float>(0.0f, 0.25f, 0.25f, 0.75f);

            chunk[1] = SORE_Graphics::MakeSprite(
                bounds, texCoords, 0.0f, texture, shader,
                GUI_LAYER, SORE_Graphics::BLEND_SUBTRACTIVE);
        }

        if(leftBorder && topBorder)
        {
            //top left corner
            bounds = SORE_Math::Rect<float>(
                0.0f,
                static_cast<float>(leftBorder),
                0.0f,
                static_cast<float>(topBorder));
            texCoords = SORE_Math::Rect<float>(0.0f, 0.25f, 0.0f, 0.25f);

            chunk[2] = SORE_Graphics::MakeSprite(
                bounds, texCoords, 0.0f, texture, shader,
                GUI_LAYER, SORE_Graphics::BLEND_SUBTRACTIVE);
        }

        if(leftBorder + rightBorder < width && topBorder)
        {
            //top border
            bounds = SORE_Math::Rect<float>(
                static_cast<float>(leftBorder),
                static_cast<float>(width - rightBorder),
                0.0f,
                static_cast<float>(topBorder));
            texCoords = SORE_Math::Rect<float>(0.25f, 0.75f, 0.0f, 0.25f);

            chunk[3] = SORE_Graphics::MakeSprite(
                bounds, texCoords, 0.0f, texture, shader,
                GUI_LAYER, SORE_Graphics::BLEND_SUBTRACTIVE);
        }

        if(rightBorder && topBorder)
        {
            //top right corner
            bounds = SORE_Math::Rect<float>(
                static_cast<float>(width - rightBorder),
                static_cast<float>(width),
                0.0f,
                static_cast<float>(topBorder));
            texCoords = SORE_Math::Rect<float>(0.75f, 1.0f, 0.0f, 0.25f);

            chunk[4] = SORE_Graphics::MakeSprite(
                bounds, texCoords, 0.0f, texture, shader,
                GUI_LAYER, SORE_Graphics::BLEND_SUBTRACTIVE);
        }

        if(rightBorder && topBorder + bottomBorder < height)
        {
            //right border
            bounds = SORE_Math::Rect<float>(
                static_cast<float>(width - rightBorder),
                static_cast<float>(width),
                static_cast<float>(topBorder),
                static_cast<float>(height - bottomBorder));
            texCoords = SORE_Math::Rect<float>(0.75f, 1.0f, 0.25f, 0.75f);

            chunk[5] = SORE_Graphics::MakeSprite(
                bounds, texCoords, 0.0f, texture, shader,
                GUI_LAYER, SORE_Graphics::BLEND_SUBTRACTIVE);
        }

        if(rightBorder && bottomBorder)
        {
            //bottom right corner
            bounds = SORE_Math::Rect<float>(
                static_cast<float>(width - rightBorder),
                static_cast<float>(width),
                static_cast<float>(height - bottomBorder),
                static_cast<float>(height));
            texCoords = SORE_Math::Rect<float>(0.75f, 1.0f, 0.75f, 1.0f);

            chunk[6] = SORE_Graphics::MakeSprite(
                bounds, texCoords, 0.0f, texture, shader,
                GUI_LAYER, SORE_Graphics::BLEND_SUBTRACTIVE);
        }

        if(bottomBorder && rightBorder + leftBorder < width)
        {
            //bottom border
            bounds = SORE_Math::Rect<float>(
                static_cast<float>(leftBorder),
                static_cast<float>(width - rightBorder),
                static_cast<float>(height - bottomBorder),
                static_cast<float>(height));
            texCoords = SORE_Math::Rect<float>(0.25f, 0.75f, 0.75f, 1.0f);

            chunk[7] = SORE_Graphics::MakeSprite(
                bounds, texCoords, 0.0f, texture, shader,
                GUI_LAYER, SORE_Graphics::BLEND_SUBTRACTIVE);
        }

        if(leftBorder && bottomBorder)
        {
            //bottom left corner
            bounds = SORE_Math::Rect<float>(
                0.0f,
                static_cast<float>(leftBorder),
                static_cast<float>(height - bottomBorder),
                static_cast<float>(height));
            texCoords = SORE_Math::Rect<float>(0.0f, 0.25f, 0.75f, 1.0f);

            chunk[8] = SORE_Graphics::MakeSprite(
                bounds, texCoords, 0.0f, texture, shader,
                GUI_LAYER, SORE_Graphics::BLEND_SUBTRACTIVE);
        }
        for(unsigned int i = 0; i < 9; ++i)
            chunk[i].SetTransform(
                SORE_Graphics::TransformationPtr(
                    new SORE_Math::Matrix4<float>(
                        GetPositionMatrix())));

    }
}
