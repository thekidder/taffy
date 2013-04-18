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

#ifndef SORE_PIPELINE_RENDERER_H
#define SORE_PIPELINE_RENDERER_H

#include <sore_buffermanager.h>
#include <sore_camera.h>
#include <sore_geometryprovider.h>
#include <sore_pipe.h>
#include <sore_profiler.h>
#include <sore_renderer.h>
#include <sore_screeninfo.h>
#include <sore_util.h>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include <stack>
#include <vector>

namespace SORE_Graphics
{
    class SORE_EXPORT PipelineRenderer : public Renderer, SORE_Utility::Noncopyable
    {
    public:
        PipelineRenderer(SORE_Profiler::Profiler* profiler_);
        ~PipelineRenderer();

        virtual void ClearGeometryProviders();
        virtual void AddGeometryProvider(GeometryProvider* gp);
        virtual void SetCameraTable(const camera_callback_table& cameras_);

        virtual bool OnResize(const SORE_Kernel::Event& e);

        virtual void Render();

        // expose the root pipe for the user to modify
        boost::shared_ptr<Pipe>& RootPipe();
    private:
        //call once per frame to keep fps numbers up to date
        void CalculateFPS();

        camera_callback_table cameras;
        std::vector<GeometryProvider*> geometry;
        boost::shared_ptr<Pipe> pipeline;

        int width, height;

        SORE_Profiler::Profiler* profiler;
    };
}

 #endif
