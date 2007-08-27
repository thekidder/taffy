
#ifndef  __SORE_RENDERER__
#define  __SORE_RENDERER__

#include "sore_kernel.h"
#include "sore_font.h"
#include "sore_allgl.h"
#include "sore_graphics.h"
#include "sore_camera.h"

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
			
			void SetSceneGraph(SORE_Graphics::SceneGraph* scene);
			void SetCamera    (SORE_Graphics::Camera*     camera);
		protected:
			int  InitializeSDL();
			int  InitializeGL();
			int InitializeSOREGraphics();
			SDL_Surface* drawContext;
			SORE_Font::font_ref font;
			
			SORE_Graphics::SceneGraph* sg;
			SORE_Graphics::Camera*     cam;
	};
}

#endif //__SORE_RENDERER__
