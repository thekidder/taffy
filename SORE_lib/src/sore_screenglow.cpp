/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//$Id$

#include  "sore_screenglow.h"

namespace SORE_Graphics
{
	ScreenGlow::ScreenGlow(unsigned int w, unsigned int h, SORE_Resource::ResourcePool& pool) : PostProcessEffect(w, h), fbo(0)
	{
		blur    = pool.GetResource<GLSLShader>("data/Shaders/gaussian.shad");
		combine = pool.GetResource<GLSLShader>("data/Shaders/combine.shad");

		fbo = new FBO(GetWidth(), GetHeight(), true, 3);
	}

	ScreenGlow::~ScreenGlow()
	{
		delete fbo;
	}

	void ScreenGlow::StartFrame(ProjectionInfo& proj)
	{
		fbo->Bind();
		fbo->SelectBuffer(0);
		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void ScreenGlow::EndFrame(ProjectionInfo& proj)
	{
		const float pixel_offset = 1.2f;

		blur->Bind();
		blur->SetUniform1i("source", 0);
		blur->SetUniform1f("offsetx", 0);
		blur->SetUniform1f("offsety", pixel_offset / GetHeight());
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		
		fbo->SelectBuffer(1);
		fbo->BindBuffer(0);
		
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		
		DrawFullscreenQuad(proj);
		
		fbo->SelectBuffer(2);
		fbo->BindBuffer(1);
		
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		
		blur->SetUniform1i("source", 0);
		blur->SetUniform1f("offsetx", pixel_offset / GetWidth());
		blur->SetUniform1f("offsety", 0.0f);
		
		DrawFullscreenQuad(proj);
		
		fbo->Unbind();
		
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		
		fbo->BindBuffers(3);
		
		combine->Bind();
		combine->SetUniform1i("tex", 0);
		combine->SetUniform1i("horz", 1);
		combine->SetUniform1i("vert", 2);
		
		DrawFullscreenQuad(proj);

		glActiveTexture(GL_TEXTURE0);
	}

	void ScreenGlow::OnScreenChange()
	{
		fbo->Resize(GetWidth(), GetHeight());
	}
}
