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
