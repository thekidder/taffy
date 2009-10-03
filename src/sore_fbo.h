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

#ifndef SORE_FBO_H
#define SORE_FBO_H

#include "sore_allgl.h"
#include "sore_dll.h"

namespace SORE_Graphics
{
	class SORE_EXPORT FBO
	{
	public:
		FBO(unsigned int w, unsigned int h, bool depthBuffer = true, unsigned int colorBuffers = 1);
		~FBO();

		void Resize(unsigned int w, unsigned int h);

		//bind color buffer in the current texture slot
		void BindBuffer(unsigned int buf);
		//bind the first num textures in the first num texture slots
		void BindBuffers(unsigned int num);
		//Bind FBO: should be called before calling the next function
		void Bind();
		//draw to a buffer
		void SelectBuffer(unsigned int buf);
		//unbind this FBOs
		void Unbind();

	private:
		void CreateBuffers();
		void DestroyBuffers();

		unsigned int width, height;
		bool depth;

		GLuint fbo, depthBuffer;

		unsigned int numColorBuffers;
		GLuint* colorBuffers;
	};
}

#endif
