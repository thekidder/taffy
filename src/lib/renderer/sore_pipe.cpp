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

#include <boost/foreach.hpp>

#include <sore_pipe.h>

SORE_Graphics::Pipe::Pipe()
{
}

SORE_Graphics::Pipe::~Pipe()
{
}

void SORE_Graphics::Pipe::AddChildPipe(Pipe* child)
{
    children.push_back(child);
}

void SORE_Graphics::Pipe::Setup()
{
    doSetup();

    pipe_vector::iterator i;
    for(i = children.begin(); i != children.end(); ++i)
    {
        i->Setup();
    }
}

void SORE_Graphics::Pipe::Render(const camera_table& cameras, render_list& list)
{
    render_list& newList = doRender(cameras, list);

    pipe_vector::iterator i;
    for(i = children.begin(); i != children.end(); ++i)
    {
        i->Render(cameras, newList);
    }
}

void SORE_Graphics::NullPipe::doSetup()
{
}

SORE_Graphics::render_list& SORE_Graphics::NullPipe::doRender
(
    const camera_table& cameras,
    render_list& list
)
{
    return list;
}

void SORE_Graphics::RenderPipe::doSetup()
{
}

SORE_Graphics::render_list& SORE_Graphics::RenderPipe::doRender
(
    const camera_table& cameras,
    render_list& list
)
{
    BOOST_FOREACH(Renderable r, list)
    {

    }

    return list;
}
