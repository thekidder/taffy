
#ifndef  __SORE_RENDERER__
#define  __SORE_RENDERER__

#include "sore_kernel.h"
#include "sore_font.h"
#include "sore_allgl.h"

namespace SORE_Kernel
{
	class Renderer : public Task
	{
		public:
			Renderer();
			~Renderer();
		
			void Frame(int elapsedTime);
			void Pause();
			void Resume();
		
			const char* GetName() const {return "Renderer";}
		protected:
			int  InitializeSDL();
			int  InitializeGL();
			int InitializeSOREGraphics();
			SDL_Surface* drawContext;
			SORE_Font::font_ref font;
	};
}

#endif //__SORE_RENDERER__
