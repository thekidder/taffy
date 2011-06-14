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

#ifndef SORE_FRAMEWIDGET_H
#define SORE_FRAMEWIDGET_H

#include <sore_texture2d.h>
#include <sore_glslshader.h>
#include <sore_widget.h>

namespace SORE_GUI
{
    enum size_mode {SCALE_ALL, SCALE_CENTER};

    class SORE_EXPORT FrameWidget : public Widget
    {
    public:
        FrameWidget(SVec s, SVec p, size_mode m, Widget* parent_ = NULL);
    protected:
        void SetBorderSizes(float l, float r, float t, float b);
        void SetTexture(SORE_Resource::Texture2DPtr tex);
        void SetShader(SORE_Resource::GLSLShaderPtr shad);

        void RenderFrame(SORE_Graphics::ImmediateModeProvider& imm_mode);
    private:
        virtual bool ProcessEvents(const SORE_Kernel::Event& e) = 0;
        virtual void UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode) = 0;

        size_mode mode;

        SORE_Resource::Texture2DPtr texture;
        SORE_Resource::GLSLShaderPtr shader;

        float leftBorder, rightBorder, topBorder, bottomBorder;
    };
}

#endif
