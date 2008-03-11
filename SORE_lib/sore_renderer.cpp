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
		fbo = img = depthbuffer = 0;
		if(_rm)
		{
			fboshad = rm->GetResource<SORE_Graphics::GLSLShader>("data/Shaders/warp_fbo.shad");
			warp = rm->GetResource<SORE_Resource::Texture2D>("data/Textures/warp.tga");
		}
		else
		{
			fboshad = NULL;
			warp = NULL;
		}
	}
	
	Renderer2D::~Renderer2D()
	{
		CleanupFBO();
	}
	
	void Renderer2D::SetupFBO()
	{
		int width = screen->width;
		int height = screen->height;
		//int width = 512;
		//int height = 512;
		
		glGenFramebuffersEXT(1, &fbo);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
		
		glGenRenderbuffersEXT(1, &depthbuffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
		
		glGenTextures(1, &img);
		glBindTexture(GL_TEXTURE_2D, img);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//  The following 3 lines enable mipmap filtering and generate the mipmap data so rendering works
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glGenerateMipmapEXT(GL_TEXTURE_2D);

		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, img, 0);
		
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if(status!=GL_FRAMEBUFFER_COMPLETE_EXT)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create FBO");
		}
		else
		{
			ENGINE_LOG(SORE_Logging::LVL_DEBUG1, "FBO created successfully");
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
	}
	
	void Renderer2D::CleanupFBO()
	{
		glDeleteFramebuffersEXT(1, &fbo);
		glDeleteTextures(1, &img);
		glDeleteRenderbuffersEXT(1, &depthbuffer);
		fbo = img = depthbuffer = 0;
	}
	
	void Renderer2D::OnScreenChange()
	{
		CleanupFBO();
		SetupFBO();
	}
	
	void IRenderer::SetScreenInfo(ScreenInfo* _screen)
	{
		screen = _screen;
		OnScreenChange();
	}
	
	void IRenderer::SetProjectionInfo(ProjectionInfo* _proj)
	{
		proj = _proj;
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
		
		SORE_Graphics::color c = {0.0,0.0,0.0,1.0};
		
		static Sprite2D glow(rm, "glow", 0.0, 0.0, -0.95, 0.0, 0.0, c);
		
		int width = screen->width;
		int height = screen->height;
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0,width,height);

		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		/*for(it=sprites.begin();it!=sprites.end();it++)
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
		}*/
		
		for(it=sprites.begin();it!=sprites.end();it++)
		{
			RenderSprite(*it);
		}

		glPopAttrib();
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		if(SORE_Graphics::GLSLShader::ShadersSupported())
			SORE_Graphics::GLSLShader::UnbindShaders();
		
		if(SORE_Graphics::GLSLShader::ShadersSupported() && fboshad && warp)
		{
			fboshad->Bind();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, img);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, warp->GetHandle());
			fboshad->SetUniform1i("fbo_img", 0);
			fboshad->SetUniform1i("warp", 1);
		}
		
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, img);
		
		glColor4f(1.0,1.0,1.0,1.0);
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.0, 0.0);
			glVertex3f(proj->left, proj->top, -0.90);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(proj->left, proj->bottom, -0.90);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(proj->right, proj->bottom, -0.90);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(proj->right, proj->top, -0.90);
		}
		glEnd();
		
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
