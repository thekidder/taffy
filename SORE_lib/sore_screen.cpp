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


#include "sore_screen.h"
#include "sore_profiler.h"
#include "sore_util.h"
#include "sore_timing.h"
#include <cassert>
#include <boost/format.hpp>
#include <functional>

SORE_Kernel::Screen::Screen(SORE_Kernel::GameKernel* gk, SORE_Graphics::ScreenInfo& _screen, std::string windowTitle, resize_callback rc, SORE_Utility::SettingsManager* _sm) : Task(gk), sm(_sm), resizeCallback(rc)
{
	ENGINE_LOG(SORE_Logging::LVL_INFO, "Creating screen");
	renderer = NULL;
	proj.type = SORE_Graphics::PERSPECTIVE;
	proj.fov = 45.0;
	proj.znear = 0.1;
	proj.zfar  = 200.0;
	proj.useScreenRatio = true;
	screen.ratio = (double)_screen.width / (double)_screen.height;
	if(InitializeSDL(windowTitle)!=0)
	{
		ENGINE_LOG(SORE_Logging::LVL_CRITICAL, boost::format("Could not initialize SDL (SDL error %s)") % SDL_GetError());
		gk->quitFlag = true;
	}
	if(sm!=NULL)
	{
		screen.width      = sm->WatchVariable("screen", "width",      boost::bind(std::mem_fun(&Screen::ChangeScreenOnSettingsChange),this));
		screen.height     = sm->WatchVariable("screen", "height",     boost::bind(std::mem_fun(&Screen::ChangeScreenOnSettingsChange),this));
		screen.fullscreen = sm->WatchVariable("screen", "fullscreen", boost::bind(std::mem_fun(&Screen::ChangeScreenOnSettingsChange),this));
		screen.resizable  = sm->WatchVariable("screen", "resizable",  boost::bind(std::mem_fun(&Screen::ChangeScreenOnSettingsChange),this));
		screen.showCursor = sm->WatchVariable("screen", "showcursor", boost::bind(std::mem_fun(&Screen::ChangeScreenOnSettingsChange),this));
	}
	SDLScreenChange(screen);
	if(InitializeGL()!=0)
	{
		ENGINE_LOG(SORE_Logging::LVL_CRITICAL, "Could not initialize GL");
		gk->quitFlag = true;
	}
	if(resizeCallback)
	{
		proj = resizeCallback(screen);
		ChangeProjectionMatrix(proj);
	}
}

void SORE_Kernel::Screen::SetResizeCallback(resize_callback callback)
{
	resizeCallback = callback;
}

void SORE_Kernel::Screen::SDLScreenChange(SORE_Graphics::ScreenInfo& _screen)
{
	double ratio = double(_screen.width)/double(_screen.height);
	screen.ratio      = ratio;
	screen.fullscreen = _screen.fullscreen;
	screen.showCursor = _screen.showCursor;
	screen.resizable  = _screen.resizable;
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
		if(videoFlags & SDL_RESIZABLE) videoFlags ^= SDL_RESIZABLE;
	Resize(_screen.width, _screen.height);
}

void SORE_Kernel::Screen::ChangeScreen(SORE_Graphics::ScreenInfo& _screen)
{
	SDL_FreeSurface(drawContext);
	SDLScreenChange(_screen);
	
	glViewport( 0, 0, ( GLsizei )screen.width, ( GLsizei )screen.height );
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	if(resizeCallback)
		proj = resizeCallback(_screen);
	
	ChangeProjectionMatrix(proj);
}

SORE_Kernel::Screen::~Screen()
{
	//if(videoFlags & SDL_FULLSCREEN)
	//	SDL_SetVideoMode(width, height, 0, videoFlags);
	SDL_ShowCursor(SDL_ENABLE);
	SDL_FreeSurface(drawContext);
	SDL_Quit();
}

void SORE_Kernel::Screen::Frame(int elapsedTime)
{
	SORE_Profiler::Sample graphics("graphics");
	if(renderer)
		renderer->Render();
	SDL_GL_SwapBuffers();
}

void SORE_Kernel::Screen::Pause()
{
}

void SORE_Kernel::Screen::Resume()
{
}

void SORE_Kernel::Screen::SetRenderer(SORE_Graphics::IRenderer* _renderer)
{
	renderer = _renderer;
}

bool SORE_Kernel::Screen::OnResize(Event* event)
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
	}
	SORE_Graphics::ScreenInfo tempScreen = screen;
	tempScreen.width = width;
	tempScreen.height = height;
	ChangeScreen(tempScreen);
	return true;
}

void SORE_Kernel::Screen::Resize(int width, int height)
{
	{
		screen.width = width;
		screen.height = height;
	}
	screen.ratio = (double)screen.width/(double)screen.height;
	drawContext = SDL_SetVideoMode(screen.width, screen.height, 0, videoFlags);
}

int SORE_Kernel::Screen::ChangeProjectionMatrix(SORE_Graphics::ProjectionInfo& projection)
{
	SetupProjection(projection);
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
			//ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("New projection is ortho2d with (%f, %f, %f, %f)") % projection.left % projection.right % projection.bottom % projection.top);
			gluOrtho2D(projection.left, projection.right, projection.top, projection.bottom);
			break;
		case SORE_Graphics::ORTHO:
			//TODO: finish ortho projection
			break;
		case SORE_Graphics::PERSPECTIVE:
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

int SORE_Kernel::Screen::InitializeSDL(std::string windowTitle)
{
	if(SDL_Init(SDL_INIT_VIDEO)!=0)
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
	SDL_WM_SetCaption(windowTitle.c_str(), windowTitle.c_str());

	return 0;
}

int SORE_Kernel::Screen::InitializeGL()
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
	char* glExtensions = (char*)glGetString(GL_EXTENSIONS);
	std::string extensions;
	if(glExtensions==NULL) extensions = "";
	else extensions = glExtensions;
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

void SORE_Kernel::Screen::SetProjection(SORE_Graphics::ProjectionInfo& info)
{
	proj = info;
	//OnResize();
	ChangeProjectionMatrix(proj);
	info.top = proj.top;
	info.bottom = proj.bottom;
	info.ratio = proj.ratio;
}

void SORE_Kernel::Screen::ChangeProjection(SORE_Graphics::ProjectionInfo& info)
{
	//OnResize();
	ChangeProjectionMatrix(info);
}

void SORE_Kernel::Screen::InitExtensions()
{
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK)
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Failed to initialize OpenGL extensions. GLEW Error: %s") % glewGetErrorString(glewError));
	}
}

GLint* SORE_Kernel::Screen::GetViewport()
{
	return viewport;
}

SORE_Graphics::ScreenInfo* SORE_Kernel::Screen::GetScreen()
{
	return &screen;
}

SORE_Graphics::ProjectionInfo SORE_Kernel::Screen::GetProjection()
{
	return proj;
}

void SORE_Kernel::Screen::ChangeScreenOnSettingsChange()
{
	int iWidth   = (int) sm->GetVariable("screen", "width");
	int iHeight  = (int) sm->GetVariable("screen", "height");
	bool bFull   = (bool)sm->GetVariable("screen", "fullscreen");
	bool bResize = (bool)sm->GetVariable("screen", "resizable");
	bool bCursor = (bool)sm->GetVariable("screen", "showcursor");
	static SORE_Graphics::ScreenInfo screen;
	screen.width=iWidth;
	screen.height=iHeight;
	screen.fullscreen=bFull;
	screen.resizable=bResize;
	screen.showCursor=bCursor;
	ChangeScreen(screen);
}

void SORE_Kernel::Screen::SetupProjection(SORE_Graphics::ProjectionInfo& pi)
{
	switch(pi.type)
	{
		case SORE_Graphics::NONE:
			break;
		case SORE_Graphics::ORTHO2D:
			if(pi.useScreenCoords)
			{
				pi.top = 0;
				pi.left = 0;
				pi.bottom = screen.height;
				pi.right = screen.width;
				pi.ratio = screen.ratio;
			}
			else if(pi.useScreenRatio)
			{
				pi.bottom = pi.left / screen.ratio;
				pi.top = pi.right / screen.ratio;
				pi.ratio = screen.ratio;
			}
			else
			{
				pi.ratio = (pi.right - pi.left) / (pi.top - pi.bottom);
			}
			break;
		case SORE_Graphics::ORTHO:
			//TODO: finish ortho projection
			break;
		case SORE_Graphics::PERSPECTIVE:
			if(pi.useScreenRatio)
			{
				pi.ratio = screen.ratio;
			}
			else
			{
				pi.ratio = (pi.right - pi.left) / (pi.top - pi.bottom);
			}
			break;
		default:
			break;
	}
}
