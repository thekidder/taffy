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

#ifndef SORE_GL_COMMAND_LIST_H
#define SORE_GL_COMMAND_LIST_H

#include <vector>

#include <sore_dll.h>
#include <sore_batch.h>
#include <sore_matrix4x4.h>
#include <sore_profiler.h>
#include <sore_renderstate.h>

namespace SORE_Graphics
{
    enum Render_command_t
    {
        CLEAR_COLOR_BUFFER,
        CLEAR_DEPTH_BUFFER,
        CLEAR_COLOR_AND_DEPTH_BUFFERS,
        SET_VIEWPORT
    };

    struct render_command_data
    {
        int d0, d1;
    };

    /*
      Generates a list of GL commands to send to the GPU from an ordered list
      of renderables. Handles state changes, draw calls, and some rudimentary
      optimizations
    */
    class SORE_EXPORT GLCommandList
    {
    public:
        GLCommandList(int w, int h);

        void AddRenderable(
            const Renderable& r,
            const geometry_entry& geometry,
            const camera_info& cam);
        void AddCommand(Render_command_t command, render_command_data data = render_command_data());

        void SetRenderbuffer(FBO* const renderbuffer_);

        void Render();

        unsigned int NumPolygons() const;
        unsigned int NumDrawCalls() const;
    private:
        void ApplyRenderCommand(Render_command_t command, render_command_data data);

        enum Render_command_type_t
        {
            RENDER,
            COMMAND
        };
        struct Command
        {
            Command(const RenderBatch& b)
            {
                type = RENDER;
                batch = b;
            }
            Command(Render_command_t c, render_command_data d)
            {
                type = COMMAND;
                command = c;
                data = d;
            }
            Render_command_type_t type;

            RenderBatch batch;
            Render_command_t command;
            render_command_data data;
        };

        //current state
        RenderState currentState;
        geometry_entry currentGeometry;

        // screen dimensions
        int width, height;

        // keep track of current render target
        FBO* renderbuffer;

        //command list
        std::vector<Command> commandList;

        //stats
        unsigned int numPolygons;
        unsigned int numDrawCalls;
    };
}

#endif
