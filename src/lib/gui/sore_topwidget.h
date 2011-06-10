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

#ifndef SORE_TOPWIDGET_H
#define SORE_TOPWIDGET_H

#include <sore_camera.h>
#include <sore_geometryprovider.h>
#include <sore_input.h>
#include <sore_widget.h>

namespace SORE_GUI
{
    class SORE_EXPORT TopWidget : public Widget
    {
    public:
        TopWidget(unsigned int width, unsigned int height);

        bool OnResize(SORE_Kernel::Event* e);

        virtual SORE_Graphics::GeometryProvider* GetGeometryProvider() { return &imm_mode; }

        SORE_Graphics::camera_info GetCamera();

        void Frame(int elapsed); // perform all logic and render
    private:
        void UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode);
        bool ProcessEvents(SORE_Kernel::Event* e);
        void UpdateResolution(unsigned int w, unsigned int h);

        SORE_Graphics::ImmediateModeProvider imm_mode;
    };
}

#endif
