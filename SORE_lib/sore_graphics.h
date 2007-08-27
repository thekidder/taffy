
#ifndef  __SORE_H__
#define  __SORE_H__

#include "sore_font.h"

namespace SORE_Graphics
{
	void Init2DOverlay();
	void UpdateViewport();

	void Init_2DCanvas();
	void Destroy_2DCanvas();
	
	void DrawString(SORE_Font::font_ref font, int x, int y, const char* fmt, ...);
	
	class SceneGraph
	{
		public:
			SceneGraph() {}
			virtual ~SceneGraph() {}
			
			virtual void Render() = 0;
	};
}

#endif /*__SORE_H__*/
