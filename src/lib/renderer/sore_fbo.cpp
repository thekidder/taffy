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

#include <sore_fbo.h>
#include <sore_logger.h>

#include <cstring> // memset

namespace SORE_Graphics
{
    FBO::FBO(unsigned int w, unsigned int h, bool depthBuffer, unsigned int colorBuffers)
        : width(w), height(h), depth(depthBuffer), numColorBuffers(colorBuffers), colorBuffers(0)
    {
        if(numColorBuffers)
        {
            this->colorBuffers = new GLuint[numColorBuffers];
        }
        CreateBuffers();
    }

    FBO::~FBO()
    {
        DestroyBuffers();
        delete[] colorBuffers;
    }

    void FBO::Resize(unsigned int w, unsigned int h)
    {
        width = w;
        height = h;
        DestroyBuffers();
        CreateBuffers();
    }

    void FBO::Bind()
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, width, height);
    }

    void FBO::Unbind()
    {
        glPopAttrib();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }

    void FBO::BindBuffer(unsigned int buf)
    {
        if(buf >= numColorBuffers)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Attempted to bind non existent buffer (%d)") % buf);
            return;
        }
        glBindTexture(GL_TEXTURE_2D, colorBuffers[buf]);
    }

    void FBO::BindBuffers(unsigned int num)
    {
        if(num > numColorBuffers)
        {
            ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Attempted to bind more buffers than exist (%d)") % num);
            num = numColorBuffers;
        }
        for(unsigned int i=0;i<num;++i)
        {
            glActiveTexture(GL_TEXTURE0+i);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        }
    }

    void FBO::SelectBuffer(unsigned int buf)
    {
        if(buf >= numColorBuffers)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Attempted to select non existent buffer (%d)") % buf);
            return;
        }
        glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT + buf);
    }

    void FBO::CreateBuffers()
    {
        glGenFramebuffersEXT(1, &fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

        if(depth)
        {
            glGenRenderbuffersEXT(1, &depthBuffer);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);

            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);
        }
        if(numColorBuffers)
            glGenTextures(numColorBuffers, colorBuffers);
        for(unsigned int i=0;i<numColorBuffers;++i)
        {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, colorBuffers[i], 0);
        }

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

        switch(status)
        {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Framebuffer creation failed: incomplete attachment");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Framebuffer creation failed: missing attachment");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Framebuffer creation failed: incomplete draw buffer");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Framebuffer creation failed: incomplete read buffer");
            break;
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            break;
        }
    }

    void FBO::DestroyBuffers()
    {
        if(fbo)
            glDeleteFramebuffersEXT(1, &fbo);
        if(depth && depthBuffer)
            glDeleteRenderbuffersEXT(1, &depthBuffer);
        glDeleteTextures(numColorBuffers, colorBuffers);
        fbo = depthBuffer = 0;
        memset(colorBuffers, 0, numColorBuffers*sizeof(GLuint));
    }
}
