 // $Id$

#ifndef  __SORE_SCREEN__
#define  __SORE_SCREEN__

#include <boost/function.hpp>

#include "sore_renderer.h"
#include "sore_kernel.h"
#include "sore_font.h"
#include "sore_allgl.h"
#include "sore_logger.h"
#include "sore_input.h"
#include "sore_settings.h"

namespace SORE_Graphics
{
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
		double ratio; //set by SORE_Screen after screen is created
		bool showCursor;
		bool fullscreen;
		bool resizable;
	};	
}

namespace SORE_Kernel
{
	typedef boost::function<SORE_Graphics::ProjectionInfo (SORE_Graphics::ScreenInfo)> resize_callback;
	class Screen : public Task
	{
		public:
			Screen(SORE_Kernel::GameKernel* gk, SORE_Graphics::ScreenInfo& _screen, std::string windowTitle="SORE App", resize_callback rc=NULL, SORE_Utility::SettingsManager* _sm=NULL);
			~Screen();
		
			void Frame(int elapsedTime);
			void Pause();
			void Resume();
		
			const char* GetName() const {return "Screen";}
			
			void SetRenderer(SORE_Graphics::IRenderer* _renderer);
			
			//both of the following change the projection matrix - the difference is that SetProjection saves it as the default
			//(this means when screen is resized, the default will be applied)
			void SetProjection(SORE_Graphics::ProjectionInfo& info);
			void ChangeProjection(SORE_Graphics::ProjectionInfo& info);
			
			void ChangeScreen(SORE_Graphics::ScreenInfo& _screen);
			
			bool OnResize(Event* event=NULL);
			
			//returns a new projection when resized
			void SetResizeCallback(boost::function<SORE_Graphics::ProjectionInfo (SORE_Graphics::ScreenInfo)> callback=NULL);
			
			GLint* GetViewport();
			SORE_Graphics::ScreenInfo* GetScreen();
			SORE_Graphics::ProjectionInfo  GetProjection();
			
			void SetupProjection(SORE_Graphics::ProjectionInfo& pi);
		protected:
			int  InitializeSDL(std::string windowTitle);
			int  InitializeGL();
			void InitExtensions();
			
			int ChangeProjectionMatrix(SORE_Graphics::ProjectionInfo& projection); //returns 0 on success
			void SDLScreenChange(SORE_Graphics::ScreenInfo& _screen);
			void ChangeScreenOnSettingsChange(); 

			
			void Resize(int width, int height); //does the actual resizing
			
			SDL_Surface* drawContext;
			int width, height; //user's previous width and height so we can reset their screen after fullscreen mode
			Uint32 videoFlags;
			
			resize_callback resizeCallback;
			
			SORE_Graphics::IRenderer* renderer;
			SORE_Graphics::ProjectionInfo proj;
			SORE_Graphics::ScreenInfo screen;
			SORE_Utility::SettingsManager* sm;
			
			GLint viewport[4];
			
	};
}

#endif //__SORE_SCREEN__
