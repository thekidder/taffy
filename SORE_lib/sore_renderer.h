
#ifndef  __SORE_RENDERER__
#define  __SORE_RENDERER__

#include "sore_kernel.h"
#include "sore_font.h"
#include "sore_allgl.h"
#include "sore_glext.h"
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
		ProjectionInfo() {type = NONE; fov = ratio = near = far = top = bottom = left = right = 0.0; useScreenCoords = useScreenRatio = false; }
		ProjectionType type;
		double fov,ratio;
		double near, far;
		double top, bottom, left, right;
		bool useScreenCoords; //if this is true, and type of projection is ortho2d, use width/height for projection
		bool useScreenRatio; //if true, uses screen ratio (for ortho, gets top/bottom by dividing left/right by ratio)
	};
}

namespace SORE_Kernel
{
	class Renderer : public Task
	{
		public:
			Renderer(SORE_Kernel::GameKernel* gk);
			~Renderer();
		
			void Frame(int elapsedTime);
			void Pause();
			void Resume();
		
			const char* GetName() const {return "Renderer";}
			
			void SetSceneGraph(SORE_Graphics::SceneGraph* scene);
			void SetCamera    (SORE_Graphics::Camera*     camera);
			
			//set default projection type
			void SetProjection(SORE_Graphics::ProjectionInfo info);
			//change current perspective - takes effect immediately
			//void ChangeProjection(SORE_Graphics::ProjectionInfo* info);
			
			bool OnResize(Event* event);
			
			GLint* GetViewport();
		protected:
			int  InitializeSDL();
			int  InitializeGL();
			void InitExtensions();
			
			int ChangeProjection(double ratio); //returns 0 on success
			
			template <typename proc>
					void glextInitProc(proc& dest, const char* name)
			{
				dest = reinterpret_cast<proc>(uglGetProcAddress(name));
				if ( !dest )
				{
					ENGINE_LOG(SORE_Logging::LVL_WARNING, "Could not set up extension %s", name);
				}
			} 
			
			SDL_Surface* drawContext;
			Uint32 videoFlags;
			SORE_Font::font_ref font;
			
			SORE_Graphics::SceneGraph* sg;
			SORE_Graphics::Camera*     cam;
			SORE_Graphics::ProjectionInfo proj;
			
			GLint viewport[4];
			
	};
}

#endif //__SORE_RENDERER__
