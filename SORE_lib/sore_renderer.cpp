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

namespace SORE_Graphics
{
	Renderer2D::Renderer2D(SORE_Resource::ResourceManager* _rm, SceneGraph2D* _scene) : scene(_scene), rm(_rm)
	{
		font = SORE_Font::LoadFont("data/Fonts/liberationmono.ttf", 24);
	}
	
	Renderer2D::~Renderer2D()
	{
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
		
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		std::vector<Sprite2D*>::iterator it;
		
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		currMaterial = NULL;
		
		SORE_Graphics::color c = {0.0,0.0,0.0,1.0};
		
		static Sprite2D glow(rm, "glow", 0.0, 0.0, -0.95, 0.0, 0.0, c);
		
		for(it=sprites.begin();it!=sprites.end();it++)
		{
			const static double relSize = 2.6;
			const static double relAlpha = 0.8;
			double offsetX = ( (*it)->width *relSize)/2.0 - (*it)->width /2.0;
			double offsetY = ( (*it)->height*relSize)/2.0 - (*it)->height/2.0;
			glow.x = (*it)->x-offsetX;
			glow.y = (*it)->y-offsetY;
			glow.c = (*it)->c;
			glow.c.a *= relAlpha;
			glow.width = (*it)->width*relSize;
			glow.height = (*it)->height*relSize;
			RenderSprite(&glow);
		}
		
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
		SORE_Graphics::DrawString(font, SORE_Graphics::GetWidth()-140, 0,  "FPS: %4.0f", fps);
		//SORE_Graphics::DrawString(font, SORE_Graphics::GetWidth()-360, SORE_Graphics::GetHeight()-30, "Speed: x%3.1f   Combo: x%3.0d", (snakes->begin())->speed()/MOVESPEED, (snakes->begin())->multiplier());
		//SORE_Graphics::DrawString(font, 0, 0, "Score: %d", int((snakes->begin())->score()));
		//if((int)(snakes->begin())->highscore()>1.0)
		//	SORE_Graphics::DrawString(font, 0, SORE_Graphics::GetHeight()-30, "high score: %d", (int)(snakes->begin())->highscore());
		SORE_Graphics::PopOverlay();
		
		GLenum error;
		while((error=glGetError())!=GL_NO_ERROR)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("GL Error: %d") % error);
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
