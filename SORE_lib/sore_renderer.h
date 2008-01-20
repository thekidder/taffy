 // $Id$

#ifndef  __SORE_RENDERER__
#define  __SORE_RENDERER__

#include "sore_kernel.h"
#include "sore_font.h"
#include "sore_allgl.h"
#include "sore_camera.h"
#include "sore_logger.h"

namespace SORE_Graphics
{
	class SceneGraph
	{
		public:
			SceneGraph() {}
			virtual ~SceneGraph() {}
			
			virtual void Render() = 0;
	};
	
	enum ProjectionType {NONE, ORTHO, ORTHO2D, PERSPECTIVE};
	
	struct ProjectionInfo
	{
		ProjectionInfo() {type = NONE; fov = ratio = znear = zfar = top = bottom = left = right = 0.0; useScreenCoords = useScreenRatio = false; }
		ProjectionType type;
		double fov,ratio;
		double znear, zfar;
		double top, bottom, left, right;
		bool useScreenCoords; //if this is true, and type of projection is ortho2d, use width/height for projection
		bool useScreenRatio; //if true, uses screen ratio (for ortho, gets top/bottom by dividing left/right by ratio)
	};
	
	struct ScreenInfo
	{
		int width, height;
		double ratio; //set by SORE_Renderer after screen is created
		bool showCursor;
		bool fullscreen;
		bool resizable;
	};	
}

namespace SORE_Kernel
{
	class Renderer : public Task
	{
		public:
			Renderer(SORE_Kernel::GameKernel* gk, SORE_Graphics::ScreenInfo& _screen);
			~Renderer();
		
			void Frame(int elapsedTime);
			void Pause();
			void Resume();
		
			const char* GetName() const {return "Renderer";}
			
			void SetSceneGraph(SORE_Graphics::SceneGraph* scene);
			void SetCamera    (SORE_Graphics::Camera*     camera);
			
			//both of the following change the projection matrix - the difference is that SetProjection saves it as the default
			//(this means when screen is resized, the default will be applied)
			void SetProjection(SORE_Graphics::ProjectionInfo& info);
			void ChangeProjection(SORE_Graphics::ProjectionInfo& info);
			
			void ChangeScreen(SORE_Graphics::ScreenInfo& _screen);
			
			bool OnResize(Event* event=NULL);
			
			GLint* GetViewport();
			SORE_Graphics::ScreenInfo* GetScreen();
			SORE_Graphics::ProjectionInfo  GetProjection();
		protected:
			int  InitializeSDL();
			int  InitializeGL();
			void InitExtensions();
			
			int ChangeProjectionMatrix(SORE_Graphics::ProjectionInfo& projection); //returns 0 on success
			void SDLScreenChange(SORE_Graphics::ScreenInfo& _screen);
			
			SDL_Surface* drawContext;
			int width,height;
			Uint32 videoFlags;
			SORE_Font::font_ref font;
			
			SORE_Graphics::SceneGraph* sg;
			SORE_Graphics::Camera*     cam;
			SORE_Graphics::ProjectionInfo proj;
			SORE_Graphics::ScreenInfo screen;
			
			GLint viewport[4];
			
	};
}

#endif //__SORE_RENDERER__
