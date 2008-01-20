//
// C++ Implementation: sore_renderer
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
// $Id$


#include "sore_renderer.h"
#include "sore_profiler.h"
#include "sore_util.h"
#include "sore_timing.h"
#include <cassert>
#include <boost/format.hpp>

SORE_Kernel::Renderer::Renderer(SORE_Kernel::GameKernel* gk, SORE_Graphics::ScreenInfo& _screen) : Task(gk)
{
	sg = NULL;
	cam = NULL;
	proj.type = SORE_Graphics::PERSPECTIVE;
	proj.fov = 45.0;
	proj.znear = 0.1;
	proj.zfar  = 200.0;
	proj.useScreenRatio = true;
	if(InitializeSDL()!=0)
	{
		ENGINE_LOG(SORE_Logging::LVL_CRITICAL, boost::format("Could not initialize SDL (SDL error %s)") % SDL_GetError());
		gk->quitFlag = true;
	}
	SDLScreenChange(_screen);
	if(InitializeGL()!=0)
	{
		ENGINE_LOG(SORE_Logging::LVL_CRITICAL, "Could not initialize GL");
		gk->quitFlag = true;
	}
}

void SORE_Kernel::Renderer::SDLScreenChange(SORE_Graphics::ScreenInfo& _screen)
{
	_screen.ratio = double(_screen.width)/double(_screen.height);
	screen = _screen;
	if(screen.fullscreen)
		videoFlags |= SDL_FULLSCREEN;
	else
		if(videoFlags & SDL_FULLSCREEN) videoFlags ^= SDL_FULLSCREEN;
	if(screen.showCursor)
		SDL_ShowCursor(SDL_ENABLE);
	else
		SDL_ShowCursor(SDL_DISABLE);
	if(screen.resizable)
		videoFlags |= SDL_RESIZABLE;
	else
		videoFlags ^= SDL_RESIZABLE;
	drawContext = SDL_SetVideoMode(screen.width, screen.height, 0, videoFlags);
}

void SORE_Kernel::Renderer::ChangeScreen(SORE_Graphics::ScreenInfo& _screen)
{
	SDL_FreeSurface(drawContext);
	SDLScreenChange(_screen);
	OnResize();
}

SORE_Kernel::Renderer::~Renderer()
{
	if(videoFlags & SDL_FULLSCREEN)
		SDL_SetVideoMode(width, height, 0, videoFlags);
	SDL_ShowCursor(SDL_ENABLE);
	SDL_FreeSurface(drawContext);
	SDL_Quit();
}

void SORE_Kernel::Renderer::Frame(int elapsedTime)
{
	SORE_Profiler::Sample graphics("graphics");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//assert(sg!=NULL && "No scene graph");
	if(sg)
	{
		if(cam)
			cam->TransformView();
		sg->Render();
	}
	SDL_GL_SwapBuffers();
}

void SORE_Kernel::Renderer::Pause()
{
}

void SORE_Kernel::Renderer::Resume()
{
}

void SORE_Kernel::Renderer::SetSceneGraph(SORE_Graphics::SceneGraph* scene)
{
	sg = scene;
}

void SORE_Kernel::Renderer::SetCamera(SORE_Graphics::Camera* camera)
{
	cam = camera;
}

bool SORE_Kernel::Renderer::OnResize(Event* event)
{
	GLint width, height;
	if(event==NULL)
	{
		width = screen.width;
		height = screen.height;
	}
	else
	{
		width  = event->resize.w;
		height = event->resize.h;
		screen.width = width;
		screen.height = height;
		screen.ratio = (double)width/(double)height;
		drawContext = SDL_SetVideoMode(width, height, 0, videoFlags);
	}
	/* Setup our viewport. */
	glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	if(ChangeProjectionMatrix(proj)!=0)
		return false;	
	return true;
}

int SORE_Kernel::Renderer::ChangeProjectionMatrix(SORE_Graphics::ProjectionInfo& projection)
{
	int returnCode = 0;
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	switch(projection.type)
	{
		case SORE_Graphics::NONE:
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "No projection type set, could not initialize projection");
			returnCode = -1;
			break;
		case SORE_Graphics::ORTHO2D:
			if(projection.useScreenCoords)
				gluOrtho2D(0, viewport[2], 0, viewport[3]);
			else
			{
				if(projection.useScreenRatio)
				{
					projection.bottom = projection.left / screen.ratio;
					projection.top = projection.right / screen.ratio;
				}
				gluOrtho2D(projection.left, projection.right, projection.top, projection.bottom);
			}
			break;
		case SORE_Graphics::ORTHO:
			//TODO: finish ortho projectionection
			break;
		case SORE_Graphics::PERSPECTIVE:
			if(projection.useScreenRatio)
			{
				projection.ratio = screen.ratio;
			}
			gluPerspective(projection.fov, projection.ratio, projection.znear, projection.zfar );
			break;
		default:
			returnCode = -1;
			break;
	}
	glMatrixMode( GL_MODELVIEW );
	/* Reset The View */
	glLoadIdentity( );
	return returnCode;
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
		ENGINE_LOG(SORE_Logging::SHOW_CRITICAL, boost::format("Video query failed: %s") % SDL_GetError());
		return 1;
	}
	//save current resolution
	width = videoInfo->current_w;
	height = videoInfo->current_h;

	videoFlags = SDL_OPENGL;
	videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
	videoFlags |= SDL_HWPALETTE; 
	/* This checks to see if surfaces can be stored in memory */
	if ( videoInfo->hw_available )
		videoFlags |= SDL_HWSURFACE;
	else
		videoFlags |= SDL_SWSURFACE;
	if(videoInfo->blit_hw)
		videoFlags |= SDL_HWACCEL;
	
	//Uint8* mask = SORE_Utility::GetBMPMask("data/snake.bmp");
	SDL_Surface* icon = SDL_LoadBMP("data/snake.bmp");
	
	SDL_WM_SetIcon(icon, NULL);
	SDL_FreeSurface(icon);
	SDL_WM_SetCaption("SNAKE!", "SNAKE!");
	return 0;
}

int SORE_Kernel::Renderer::InitializeGL()
{
	//OpenGL setup
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	/* Enable smooth shading */
	glShadeModel( GL_SMOOTH );
	/* Set the background black */
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	/* Depth buffer setup */
	glClearDepth( 1.0f );
	/* Enables Depth Testing */
	glEnable( GL_DEPTH_TEST );
	/* The Type Of Depth Test To Do */
	glDepthFunc( GL_LEQUAL );
	/* Really Nice Perspective Calculations */
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	if(!OnResize())
	{
		return 1;
	}
	InitExtensions();
	ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("OpenGL Rendering information\nRenderer   : %s\nVender     : %s\nAPI Version: %s") % (char*)glGetString(GL_RENDERER) % (char*)glGetString(GL_VENDOR) % (char*)glGetString(GL_VERSION));
	std::string extensions = (char*)glGetString(GL_EXTENSIONS);
	int pos;
	while((pos=extensions.find(" "))!=std::string::npos)
	{
		extensions.replace(pos, 1, "\n");
	}
	ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("OpenGL extension string:\n%s") % extensions);
#ifdef WIN32
	if(WGLEW_EXT_swap_control)
	{
		wglSwapIntervalEXT(0); //turn off vsync
	}
	else
	{
		ENGINE_LOG(SORE_Logging::LVL_WARNING, "Vsync control not available");
	}
#endif
	return 0;
}

void SORE_Kernel::Renderer::SetProjection(SORE_Graphics::ProjectionInfo& info)
{
	proj = info;
	//OnResize();
	ChangeProjectionMatrix(proj);
	info.top = proj.top;
	info.bottom = proj.bottom;
	info.ratio = proj.ratio;
}

void SORE_Kernel::Renderer::ChangeProjection(SORE_Graphics::ProjectionInfo& info)
{
	//OnResize();
	ChangeProjectionMatrix(info);
}

void SORE_Kernel::Renderer::InitExtensions()
{
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK)
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Failed to initialize OpenGL extensions. GLEW Error: %s") % glewGetErrorString(glewError));
	}
}

GLint* SORE_Kernel::Renderer::GetViewport()
{
	return viewport;
}

SORE_Graphics::ScreenInfo* SORE_Kernel::Renderer::GetScreen()
{
	return &screen;
}

SORE_Graphics::ProjectionInfo SORE_Kernel::Renderer::GetProjection()
{
	return proj;
}
