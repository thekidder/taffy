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

#include <sore_gl_command_list.h>

SORE_Graphics::GLCommandList::GLCommandList(int w, int h)
    : renderbuffer(0), width(w), height(h)
{
}

void SORE_Graphics::GLCommandList::AddRenderable(const Renderable& r, const geometry_entry& geometry, const camera_info& cam)
{
    RenderState state(r, cam);
    // apply current renderbuffer
    state.SetRenderbuffer(renderbuffer);
    RenderState difference = state.Difference(currentState);

    if(!difference.Renderbuffer() && currentState.Renderbuffer())
    {
        render_command_data data;
        data.d0 = width;
        data.d1 = height;
        commandList.push_back(Command(SET_VIEWPORT, data));
    }

    if(currentGeometry.geometry != geometry.geometry || commandList.back().type == COMMAND)
    {
        commandList.push_back(RenderBatch(geometry, difference, true));
    }
    else if(!difference.Empty())
    {
        commandList.push_back(RenderBatch(geometry, difference, false));
    }
    else
    {
        commandList.back().batch.AddIndices(geometry);
    }

    currentState = state;
    currentGeometry = geometry;
}

void SORE_Graphics::GLCommandList::AddCommand(Render_command_t command, render_command_data data)
{
    // if we add a command, make sure to push the current state first
    // e.x, when we issue a clear command, we want to make sure the current
    // FBO is bound
    currentState.SetRenderbuffer(renderbuffer);
    commandList.push_back(RenderBatch(currentState));

    commandList.push_back(Command(command, data));
}

void SORE_Graphics::GLCommandList::SetRenderbuffer(FBO* const renderbuffer_)
{
    renderbuffer = renderbuffer_;
}

void SORE_Graphics::GLCommandList::Render()
{
    numPolygons = 0;
    numDrawCalls = 0;

    std::vector<Command>::iterator it;
    RenderBatch* lastRendered = 0;
    for(it = commandList.begin(); it != commandList.end(); ++it)
    {
        if(it->type == RENDER)
        {
            numPolygons += it->batch.Render();
            numDrawCalls++;
            lastRendered = &it->batch;
        }
        else
        {
            ApplyRenderCommand(it->command, it->data);
        }
    }
    if(lastRendered)
    {
        lastRendered->EndDraw();
    }
}

unsigned int SORE_Graphics::GLCommandList::NumPolygons() const
{
    return numPolygons;
}

unsigned int SORE_Graphics::GLCommandList::NumDrawCalls() const
{
    return numDrawCalls;
}

void SORE_Graphics::GLCommandList::ApplyRenderCommand(Render_command_t command, render_command_data data)
{
    switch(command)
    {
    case CLEAR_DEPTH_BUFFER:
        glClear(GL_DEPTH_BUFFER_BIT);
        break;
    case CLEAR_COLOR_BUFFER:
        glClear(GL_COLOR_BUFFER_BIT);
        break;
    case CLEAR_COLOR_AND_DEPTH_BUFFERS:
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        break;
    case SET_VIEWPORT:
        glViewport(0, 0, data.d0, data.d1);
        break;
    default:
        break;
    }
}