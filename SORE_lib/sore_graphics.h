// $Id$

#ifndef  __SORE_H__
#define  __SORE_H__

#include "sore_renderer.h"
#include "sore_font.h"
#include "sore_logger.h"
#include "sore_input.h"

namespace SORE_Graphics
{
	void SetRenderer(SORE_Kernel::Renderer* _r);
	
	void DrawString(SORE_Font::font_ref font, int x, int y, const char* fmt, ...);
	
	void PushProjection(SORE_Graphics::ProjectionInfo& info);
	void PopProjection();
	
	void PushOverlay();
	void PopOverlay();
	
	int  GetHeight();
	int  GetWidth();
}

#endif /*__SORE_H__*/
