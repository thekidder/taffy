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

#ifndef SORE_PIPELINEITEM_H
#define SORE_PIPELINEITEM_H

#include <vector>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/unordered_map.hpp>

#include <sore_camera.h>
#include <sore_dll.h>
#include <sore_renderable.h>

namespace SORE_Graphics
{
    typedef std::vector<Renderable> render_list;

    class SORE_EXPORT Pipe
    {
    public:
        Pipe();
        virtual ~Pipe();

        /*
          Tree functions: Used to build a tree of pipes for the renderer to
          iterate through and render the scene. Pipes maintain ownership of their
          children and delete them when they are deconstructed. Always call as:
          AddChildPipe(new SomePipeClass(...));
        */
        void AddChildPipe(Pipe* pipe);

        void Setup();
        void Render(const camera_table& cameras, render_list& list);
    protected:
        /*
          Virtual setup/render functions to be implemented by Pipe implementations
        */
        virtual void doSetup() = 0;
        virtual render_list& doRender(const camera_table& cameras, render_list& list) = 0;
    private:
        typedef boost::ptr_vector<Pipe> pipe_vector;
        pipe_vector children;
    };

    /*
      Collection of basic pipes:
        NullPipe: does nothing; passes geometry through
        RenderPipe: renders geometry with the default shader
        SortingPipe: sorts geometry according to a callback
    */

    class SORE_EXPORT NullPipe : public Pipe
    {
    protected:
        virtual void doSetup();
        virtual render_list& doRender(const camera_table& cameras, render_list& list);
    };

    class SORE_EXPORT RenderPipe: public Pipe
    {
    protected:
        virtual void doSetup();
        virtual render_list& doRender(const camera_table& cameras, render_list& list);
    }
}

#endif
