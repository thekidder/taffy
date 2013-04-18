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
#include <sore_texture2d.h>

#include <cstring> // memset

namespace SORE_Graphics
{
    FBO::FBO(unsigned int w, unsigned int h, bool depthBuffer, unsigned int colorBuffers)
        : width(w), height(h), depth(depthBuffer)
    {
        CreateBuffers(colorBuffers);
    }

    FBO::FBO(SORE_Resource::Texture2DPtr texture)
        : width(texture->Width()), height(texture->Height()), depth(false)
    {
        colorBuffers.push_back(texture);
        CreateBuffers(1);
    }

    FBO::FBO(const std::vector<SORE_Resource::Texture2DPtr>& textures)
        : width(0), height(0), depth(false)
    {
        if(textures.size() > 0)
        {
            width = textures.back()->Width();
            height = textures.back()->Height();
        }

        colorBuffers = textures;
        CreateBuffers(colorBuffers.size());
    }

    FBO::~FBO()
    {
        DestroyBuffers();
    }

    void FBO::Resize(unsigned int w, unsigned int h)
    {
        width = w;
        height = h;
        unsigned int numColorBuffers = colorBuffers.size();
        DestroyBuffers();
        CreateBuffers(numColorBuffers);
    }

    void FBO::Bind()
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
        glViewport(0, 0, width, height);
    }

    void FBO::Unbind()
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }

    void FBO::BindBuffer(unsigned int buf)
    {
        if(buf >= colorBuffers.size())
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Attempted to bind non existent buffer (%d)") % buf);
            return;
        }
        glBindTexture(GL_TEXTURE_2D, colorBuffers[buf]->Handle());
    }

    void FBO::BindBuffers(unsigned int num)
    {
        if(num > colorBuffers.size())
        {
            ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Attempted to bind more buffers than exist (%d)") % num);
            num = colorBuffers.size();
        }
        for(unsigned int i=0; i<num; ++i)
        {
            glActiveTexture(GL_TEXTURE0+i);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]->Handle());
        }
    }

    void FBO::Draw()
    {
        static GLenum buffers[] = {
            GL_COLOR_ATTACHMENT0_EXT,
            GL_COLOR_ATTACHMENT1_EXT,
            GL_COLOR_ATTACHMENT2_EXT,
            GL_COLOR_ATTACHMENT3_EXT,
            GL_COLOR_ATTACHMENT4_EXT,
            GL_COLOR_ATTACHMENT5_EXT,
            GL_COLOR_ATTACHMENT6_EXT,
            GL_COLOR_ATTACHMENT7_EXT
        };
        if(colorBuffers.size())
            glDrawBuffers(colorBuffers.size(), buffers);
    }

    void FBO::CreateBuffers(unsigned int numColorBuffers)
    {
        glGenFramebuffersEXT(1, &fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

        CreateTextures(numColorBuffers);

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

    void FBO::CreateTextures(unsigned int numColorBuffers)
    {
        if(depth)
        {
            if(!depthBuffer)
            {
                depthBuffer = new SORE_Resource::Texture2D(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, width, height);
                depthBuffer->MinFilter(GL_NEAREST);
                depthBuffer->MagFilter(GL_NEAREST);
            }

            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthBuffer->Handle(), 0);
        }

        if(colorBuffers.size() != numColorBuffers)
        {
            colorBuffers.clear();
            for(unsigned int i=0;i<numColorBuffers;++i)
            {
                SORE_Resource::Texture2DPtr t = new SORE_Resource::Texture2D(GL_RGBA8, GL_RGBA, width, height);
                colorBuffers.push_back(t);
            }
        }
        for(size_t i = 0; i < colorBuffers.size(); ++i)
        {
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, colorBuffers[i]->Handle(), 0);
        }
    }

    void FBO::DestroyBuffers()
    {
        if(fbo)
            glDeleteFramebuffersEXT(1, &fbo);
        fbo = 0;
        depthBuffer = SORE_Resource::Texture2DPtr();
        colorBuffers.clear();
    }
    
    unsigned int FBO::Handle() const
    {
        if(depth)
            return depthBuffer->Handle();
        else if(colorBuffers.size())
            return colorBuffers[0]->Handle();
        else
            return 0;
    }
}
