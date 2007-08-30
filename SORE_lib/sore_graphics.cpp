//
// C++ Implementation: sore_graphics
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "sore_graphics.h"
#include "sore_logger.h"
#include "sore_allgl.h"
#include <cassert>

namespace SORE_Graphics
{
	void WindowToReal(int* window, int* real);
	
	GLint viewport[4];
}

void SORE_Graphics::WindowToReal(int* window, int* real)
{
	if(window[0]>viewport[2] || window[1]>viewport[3] || window[0]<0 || window[1]<0)
		return;
	real[0] = window[0];
	real[1] = viewport[3]-window[1];
}

void SORE_Graphics::Init2DOverlay()
{
	UpdateViewport();
}

void SORE_Graphics::UpdateViewport()
{
	glGetIntegerv(GL_VIEWPORT, viewport);
	//std::cout << viewport[0] << ":" << viewport[1] << ":" << viewport[2] << ":" << viewport[3] << std::endl;
	ENGINE_LOG(SORE_Logging::LVL_DEBUG2, "OpenGL viewport: (%d, %d, %d, %d)", viewport[0], viewport[1], viewport[2], viewport[3]);
}

void SORE_Graphics::Init_2DCanvas()
{
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//glOrtho(viewport[0],viewport[2],viewport[3],viewport[1], -1.0f, -1.0f);
	gluOrtho2D(viewport[0],viewport[2],viewport[1],viewport[3]);
	glPopAttrib();
	glLoadIdentity();
}

void SORE_Graphics::DrawString(SORE_Font::font_ref font, int x, int y, const char* fmt, ...)
{
	int window[2];
	window[0] = x;
	window[1] = y;
	int real[2];
	
	WindowToReal(window, real);
	
	
	int h = SORE_Font::FontHeight(font);
	
	//std::cout << "height: " << h << " " << real[1] << std::endl;
	
	real[1] -= h;
	assert(real[1]>=0);
	char text[256];
		
	if(fmt!=NULL)
	{
		va_list ap;
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
		SORE_Font::Print(font, real[0], real[1], text);
	}
}

void SORE_Graphics::Destroy_2DCanvas()
{
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}

/*inline void SORE_Font::PushScreenCoordMatrix()
{
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(viewport[0],viewport[2],viewport[1],viewport[3]);
	glPopAttrib();
}

inline void SORE_Font::PopProjectionMatrix()
{
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}*/
