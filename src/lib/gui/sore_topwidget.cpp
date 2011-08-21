/**************************************************************************
 * Copyright 2011 Adam Kidder. All rights reserved.                       *
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

#include <boost/foreach.hpp>

#include <sore_logger.h>
#include <sore_profiler.h>
#include <sore_sample.h>
#include <sore_topwidget.h>

namespace SORE_GUI
{
    TopWidget::TopWidget(
        SORE_Resource::Font_cache_t& fontCache,
        SORE_Resource::Shader_cache_t& shaderCache,
        SORE_Resource::Texture_cache_t& textureCache,
        SORE_Profiler::Profiler* profiler_)
        : Widget(fontCache, shaderCache, textureCache,
        // bogus initial size until we receive a resize event
        SVec(SUnit(400), SUnit(400)), SVec(), NULL),
        profiler(profiler_),
        imm_mode(SORE_Resource::GLSLShaderPtr())  
    {
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

    void TopWidget::Frame(int elapsed)
    {
        PROFILE_BLOCK("GUI Update", profiler);

        imm_mode.Start();
        imm_mode.SetKeywords("gui");
        Widget::Frame(elapsed, imm_mode);
    }

    void TopWidget::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
    {
    }

    bool TopWidget::ProcessEvents(const SORE_Kernel::Event& e)
    {
        if(e.type == SORE_Kernel::RESIZE)
        {
            UpdateResolution(e.resize.w, e.resize.h);
            imm_mode.OnResize(e);
            return true;
        }
        return false;
    }
}
