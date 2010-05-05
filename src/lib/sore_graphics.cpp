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
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
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

#include <sore_allgl.h>
#include <sore_graphics.h>

namespace SORE_Graphics
{
	void DrawFullscreenQuad(ProjectionInfo& proj)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.0f, 0.0f); 
			glVertex3f(proj.left, proj.top, 0.0f);
			glTexCoord2f(0.0f, 1.0f); 
			glVertex3f(proj.left, proj.bottom, 0.0f);
			glTexCoord2f(1.0f, 1.0f); 
			glVertex3f(proj.right, proj.bottom, 0.0f);
			glTexCoord2f(1.0f, 0.0f); 
			glVertex3f(proj.right, proj.top, 0.0f);
		}
		glEnd();
	}

	void PrintGLErrors(unsigned int logLevel)
	{
		GLenum error;
    while((error=glGetError())!=GL_NO_ERROR)
		{
			switch(error)
			{
			case GL_INVALID_ENUM:
				ENGINE_LOG(logLevel, "GL Error: invalid enum");
				break;
			case GL_INVALID_VALUE:
				ENGINE_LOG(logLevel, "GL Error: invalid value");
				break;
			case GL_INVALID_OPERATION:
				ENGINE_LOG(logLevel, "GL Error: invalid operation");
				break;
			case GL_STACK_OVERFLOW:
				ENGINE_LOG(logLevel, "GL Error: stack overflow");
				break;
			case GL_STACK_UNDERFLOW:
				ENGINE_LOG(logLevel, "GL Error: stack underflow");
				break;
			case GL_OUT_OF_MEMORY:
				ENGINE_LOG(logLevel, "GL Error: out of memory");
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
				ENGINE_LOG(logLevel, "GL Error: invalid framebuffer operation");
				break;
			default:
				ENGINE_LOG(logLevel, "GL Error: unknown error");
				break;
			}
		}

	}

}
