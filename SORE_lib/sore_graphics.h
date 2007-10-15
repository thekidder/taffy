
#ifndef  __SORE_H__
#define  __SORE_H__

#include "sore_glext.h"
#include "sore_font.h"
#include "sore_logger.h"
#include "sore_input.h"

namespace SORE_Graphics
{
	void Init2DOverlay();
	void UpdateViewport();

	void Init_2DCanvas();
	void Destroy_2DCanvas();
	
	bool OnResize(SORE_Kernel::Event* event);
	void DrawString(SORE_Font::font_ref font, int x, int y, const char* fmt, ...);
	
	void InitExtensions();
	
	template <typename proc>
	void glextInitProc(proc& dest, const char* name)
	{
		dest = reinterpret_cast<proc>(uglGetProcAddress(name));
		if ( !dest )
		{
			ENGINE_LOG(SORE_Logging::LVL_WARNING, "Could not set up extension %s", name);
		}
	} 
	
	class SceneGraph
	{
		public:
			SceneGraph() {}
			virtual ~SceneGraph() {}
			
			virtual void Render() = 0;
	};
}

#endif /*__SORE_H__*/
