/***************************************************************************
 *   Copyright (C) 2008 by Adam Kidder                                     *
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
// $Id$

#include "sore_renderer.h"
#include "sore_graphics.h"

#include <boost/format.hpp>

namespace SORE_Graphics
{
	Renderer2D::Renderer2D(SORE_Resource::ResourcePool* _rm, SceneGraph2D* _scene) : scene(_scene), rm(_rm)
	{
		if(rm)
		{
			std::string path = SORE_Font::FontPaths::GetFontPath("arial.ttf");
			printFPS = rm->GetResource<SORE_Font::Font>(path, "32");
		}
	}
	
	Renderer2D::~Renderer2D()
	{
	}
	
	void Renderer2D::OnScreenChange()
	{
	}
	
	void IRenderer::SetScreenInfo(ScreenInfo _screen)
	{
		screen = _screen;
		OnScreenChange();
	}
	
	void IRenderer::SetProjectionInfo(ProjectionInfo _proj)
	{
		proj = _proj;
		OnProjectionChange();
	}
	
	void Renderer2D::SetSpriteList(std::vector<Sprite2D*> s)
	{
		sprites = s;
	}
			
	void Renderer2D::Render()
	{
		SetSpriteList(scene->GetRenderList());
		static int frames = 0;
		static int T0 = SORE_Timing::GetGlobalTicks();
		static float fps;
		
		std::vector<Sprite2D*>::iterator it;
		
		currMaterial = NULL;
		
		SORE_Graphics::color c(0.0f,0.0f,0.0f,1.0f);
				
		int width = screen.width;
		int height = screen.height;
		
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		for(it=sprites.begin();it!=sprites.end();it++)
		{
			RenderSprite(*it);
		}
		
		frames++;
		{
			GLint t = SORE_Timing::GetGlobalTicks();
			if (t - T0 >= 500) //calculate FPS every 50 milliseconds
			{
				GLfloat seconds = (GLfloat)((t - T0) / 10000.0);
				fps = frames / seconds;
				T0 = t;
				frames = 0;
			}
		}
	
		if(SORE_Graphics::GLSLShader::ShadersSupported())
			SORE_Graphics::GLSLShader::UnbindShaders();

		SORE_Graphics::PushOverlay();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		//printFPS->Print(0, 0,  boost::format("FPS: %4.0f") % fps);
		printFPS->Print(0, 0,  boost::format("this is a very very very very very\nlong line of text that is separated by a newline."));
		SORE_Graphics::PopOverlay();
		
		GLenum error;
		while((error=glGetError())!=GL_NO_ERROR)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Renderer: GL Error: %d") % error);
		}
	}
	
	void Renderer2D::RenderSprite(Sprite2D* s)
	{
		if(s->m != currMaterial)
		{
			s->m->Bind();
			currMaterial = s->m;
		}
		glBegin(GL_QUADS);
		glColor4f(s->c.r, s->c.g, s->c.b, s->c.a);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(s->x, s->y+s->height, s->depth);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(s->x, s->y, s->depth);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(s->x+s->width, s->y, s->depth);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(s->x+s->width, s->y+s->height, s->depth);
		glEnd();
	}
}
