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
	Renderer2D::Renderer2D()
	{
		font = SORE_Font::LoadFont("data/Fonts/liberationmono.ttf", 24);
	}
	
	Renderer2D::~Renderer2D()
	{
	}
			
	gc_id Renderer2D::AddRenderable(IRenderable gc)
	{
		gc_id id;
		if(!unusedIds.empty())
		{
			id = *unusedIds.begin();
			unusedIds.pop_front();
		}
		else
			id = geometry.size();
		geometry.insert(std::pair<gc_id, IRenderable>(id, gc));
		return id;
	}
	
	IRenderable* Renderer2D::GeometryChunkPtr(gc_id id)
	{
		return &(geometry[id]);
	}
	
	void Renderer2D::RemoveGeometryChunk(gc_id gc)
	{
		geometry.erase(gc);
		unusedIds.push_back(gc);
	}
			
	void Renderer2D::Render()
	{
		static int frames = 0;
		static int T0 = SORE_Timing::GetGlobalTicks();
		static float fps;
		
		/*glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		std::map<gc_id, IRenderable>::iterator it;
		for(it=geometry.begin();it!=geometry.end();it++)
		{
			it->second.Draw();
		}*/
		/*GLuint currTex = 0;
		GLSLShader* currShader = NULL;
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		std::map<gc_id, IRenderable*>::iterator it;
		glBegin(GL_QUADS);
		for(it=geometry.begin();it!=geometry.end();it++)
		{
			if(it->second.shader != currShader)
			{
				glEnd();
				it->second.shader->Bind();
				currShader = it->second.shader;
				glBegin(GL_QUADS);
			}
			if(it->second.texture != currTex)
			{
				glEnd();
				glBindTexture(GL_TEXTURE_2D, it->second.texture);
				currTex = it->second.texture;
				glBegin(GL_QUADS);
			}
			glColor4f(it->second.r, it->second.g, it->second.b, it->second.a);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(it->second.x, it->second.y+it->second.height, it->second.depth);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(it->second.x, it->second.y, it->second.depth);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(it->second.x+it->second.width, it->second.y, it->second.depth);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(it->second.x+it->second.width, it->second.y+it->second.height, it->second.depth);
			
		}
		glEnd();*/
		
		
		/*frames++;
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
	
		SORE_Graphics::GLSLShader::UnbindShaders();

		SORE_Graphics::PushOverlay();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		SORE_Graphics::DrawString(font, SORE_Graphics::GetWidth()-140, 0,  "FPS: %4.0f", fps);
		//SORE_Graphics::DrawString(font, SORE_Graphics::GetWidth()-360, SORE_Graphics::GetHeight()-30, "Speed: x%3.1f   Combo: x%3.0d", (snakes->begin())->speed()/MOVESPEED, (snakes->begin())->multiplier());
		//SORE_Graphics::DrawString(font, 0, 0, "Score: %d", int((snakes->begin())->score()));
		//if((int)(snakes->begin())->highscore()>1.0)
		//	SORE_Graphics::DrawString(font, 0, SORE_Graphics::GetHeight()-30, "high score: %d", (int)(snakes->begin())->highscore());
		SORE_Graphics::PopOverlay();
		*/
	}
}
