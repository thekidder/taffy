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

#ifndef SORE_SLIDERWIDGET_H
#define SORE_SLIDERWIDGET_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <boost/signals.hpp>
#include <boost/function.hpp>

#include <sore_resource.h>
#include <sore_framewidget.h>

namespace SORE_GUI
{
    class SORE_EXPORT SliderWidget : public FrameWidget
    {
    public:
        SliderWidget(SVec s, SVec p, int min, int max,
                     SORE_Resource::ResourcePool& pool, Widget* par=NULL);
        ~SliderWidget();

        void ConnectChange(boost::function<void (int)> c);

        int GetValue() const;
        void SetValue(int value);
    private:
        //TODO:fixme
        //virtual void UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode);
        bool ProcessEvents(SORE_Kernel::Event* e);
        void UpdatePosition();
        void UpdateSlider();

        float ValueToX(int value) const;
        int XToValue(float x) const;

        SORE_Graphics::Texture2DPtr bg, slider;
        SORE_Graphics::GLSLShaderPtr shader;
        SORE_Graphics::GeometryChunk* sliderChunk;
        SORE_Math::Matrix4<float> sliderMat;

        boost::signal<void (int)> onChange;
        bool dragged;

        int minimum, maximum, current;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
