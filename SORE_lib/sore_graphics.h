
#ifndef  __SORE_H__
#define  __SORE_H__

#include "font.h"

namespace SORE_2DOverlay
{
	void Init();
	void Init_2DCanvas();
	void Destroy_2DCanvas();
	void UpdateViewport();
	
	void DrawString(SORE_Font::font_ref font, int x, int y, const char* fmt, ...);
}

#endif /*__SORE_H__*/
