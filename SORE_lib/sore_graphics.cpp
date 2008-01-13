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
	SORE_Kernel::Renderer* r;
	ProjectionInfo old;
	void WindowToReal(int* window, int* real);
}

void SORE_Graphics::SetRenderer(SORE_Kernel::Renderer* _r) 
{
	r = _r;
}

void SORE_Graphics::PushProjection(SORE_Graphics::ProjectionInfo& info)
{
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	r->SetProjection(info);
	glPopAttrib();
	glLoadIdentity();
}

void SORE_Graphics::PopProjection()
{
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	r->SetProjection(old);
}

void SORE_Graphics::PushOverlay()
{
	old = r->GetProjection();
	static SORE_Graphics::ProjectionInfo overlay;
	overlay.type   = SORE_Graphics::ORTHO2D;
	overlay.left   = 0;
	overlay.right  = r->GetScreen()->width;
	overlay.top    = 0;
	overlay.bottom = r->GetScreen()->height;
	overlay.useScreenRatio = false;
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
		return;
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
