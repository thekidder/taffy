
#ifndef  __SORE_RENDERER__
#define  __SORE_RENDERER__

#include "sore_kernel.h"
#include "allgl.h"

namespace SORE_Kernel
{
	class Renderer : public Task
	{
		public:
			Renderer();
			~Renderer();
		
			void Frame();
			void Pause();
			void Resume();
		
			const char* GetName() const {return "Renderer";}
		protected:
			int  InitializeSDL();
			int  InitializeGL();
			SDL_Surface* drawContext;
	};
}

#endif //__SORE_RENDERER__
