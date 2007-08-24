
#include "sore_renderer.h"
#include "sore_logger.h"

SORE_Kernel::Renderer::Renderer()
{
	if(InitializeSDL()!=0)
	{
		ENGINE_LOG(SORE_Logging::CRITICAL, "Could not initialize SDL (SDL error %s)", SDL_GetError());
	}
	if(InitializeGL()!=0)
	{
		ENGINE_LOG_S(SORE_Logging::CRITICAL, "Could not initialize GL");
	}
}

SORE_Kernel::Renderer::~Renderer()
{
}

void SORE_Kernel::Renderer::Frame()
{
}

void SORE_Kernel::Renderer::Pause()
{
}

void SORE_Kernel::Renderer::Resume()
{
}

int SORE_Kernel::Renderer::InitializeSDL()
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)!=0)
	{
		return 1;
	}	
	
	const SDL_VideoInfo *videoInfo;
	/* Fetch the video info */
	videoInfo = SDL_GetVideoInfo( );

	if ( !videoInfo )
	{
		ENGINE_LOG(SORE_Logging::CRITICAL, "Video query failed: %s", SDL_GetError());
		return 1;
	}

	Uint32 videoFlags = SDL_OPENGL;
	//if(FULLSCREEN) videoFlags |= SDL_FULLSCREEN;
	//if(RESIZEABLE) videoFlags |= SDL_RESIZABLE;
	videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
	videoFlags |= SDL_HWPALETTE; 
	/* This checks to see if surfaces can be stored in memory */
	if ( videoInfo->hw_available )
		videoFlags |= SDL_HWSURFACE;
	else
		videoFlags |= SDL_SWSURFACE;

	/* This checks if hardware blits can be done */
	if ( videoInfo->blit_hw )
		videoFlags |= SDL_HWACCEL;
	drawContext = SDL_SetVideoMode(640, 480, 0, videoFlags);
	return 0;
}

int SORE_Kernel::Renderer::InitializeGL()
{
	return 1;
}
