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
// $Id$


#include "sore_graphics.h"
#include "sore_logger.h"
#include "sore_allgl.h"
#include <cassert>

namespace SORE_Graphics
{
	SORE_Kernel::Screen* r;
	//ProjectionInfo old;
	void WindowToReal(int* window, int* real);
}

void SORE_Graphics::SetScreen(SORE_Kernel::Screen* _r) 
{
	r = _r;
}

void SORE_Graphics::PushProjection(SORE_Graphics::ProjectionInfo& info)
{
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	r->ChangeProjection(info);
	glPopAttrib();
	glLoadIdentity();
}

void SORE_Graphics::PopProjection()
{
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	//r->ChangeProjection(old);
}

void SORE_Graphics::PushOverlay()
{
	//old = r->GetProjection();
	static SORE_Graphics::ProjectionInfo overlay;
	overlay.type   = SORE_Graphics::ORTHO2D;
	overlay.useScreenCoords = true;
	PushProjection(overlay);
}

void SORE_Graphics::PopOverlay()
{
	PopProjection();
}

int SORE_Graphics::GetWidth()
{
	return r->GetScreen()->width;
}

int SORE_Graphics::GetHeight()
{
	return r->GetScreen()->height;
}

void SORE_Graphics::WindowToReal(int* window, int* real)
{
	GLint* viewport = r->GetViewport();
	if(window[0]>viewport[2] || window[1]>viewport[3] || window[0]<0 || window[1]<0)
	{
		ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Viewport is inconsistent: (%d, %d, %d, %d) to (%d, %d)") % viewport[0] % viewport[1] % viewport[2] % viewport[3] % window[0] % window[1]);
		return;
	}
	real[0] = window[0];
	real[1] = viewport[3]-window[1];
}

void SORE_Graphics::DrawString(SORE_Font::font_ref font, int x, int y, const char* fmt, ...)
{
	int window[2];
	int real[2];
	window[0] = x;
	window[1] = y;
	real[0] = 0;
	real[1] = 0;

	SORE_Graphics::WindowToReal(window, real);
	
	int h = SORE_Font::FontHeight(font);
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

void SORE_Graphics::ScreenAlignedQuad()
{
	glMatrixMode (GL_MODELVIEW); 
	glPushMatrix (); 
	glLoadIdentity (); 
	glMatrixMode (GL_PROJECTION); 
	glPushMatrix (); 
	glLoadIdentity ();
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex3i(-1, -1, -1);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3i(-1, 1, -1);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3i(1, 1, -1);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3i(1, -1, -1);
	}
	glEnd();
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
}
