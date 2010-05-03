/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "sore_fbo.h"
#include "sore_logger.h"

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
