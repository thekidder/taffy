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
//

#include "sore_renderer.h"
#include "sore_profiler.h"
#include "sore_util.h"
#include "sore_timing.h"
#include <cassert>

SORE_Kernel::Renderer::Renderer(SORE_Kernel::GameKernel* gk, SORE_Graphics::ScreenInfo& _screen) : Task(gk)
{
	sg = NULL;
	cam = NULL;
	proj.type = SORE_Graphics::PERSPECTIVE;
	proj.fov = 45.0;
	proj.znear = 0.1;
	proj.zfar  = 200.0;
	proj.useScreenRatio = true;
	_screen.ratio = double(_screen.width)/double(_screen.height);
	screen = _screen;
	if(InitializeSDL()!=0)
	{
		ENGINE_LOG(SORE_Logging::LVL_CRITICAL, "Could not initialize SDL (SDL error %s)", SDL_GetError());
		gk->quitFlag = true;
	}
	if(InitializeGL()!=0)
	{
		ENGINE_LOG_S(SORE_Logging::LVL_CRITICAL, "Could not initialize GL");
		gk->quitFlag = true;
	}

	/*font = SORE_Font::LoadFont("data/Fonts/liberationmono.ttf", 24);
	if(font == 0)
	{
		ENGINE_LOG_S(SORE_Logging::LVL_ERROR, "Could not load renderer font");
		gk->quitFlag = true;
	}*/
}

SORE_Kernel::Renderer::~Renderer()
{
	if(videoFlags & SDL_FULLSCREEN)
		SDL_SetVideoMode(width, height, 0, videoFlags);
	SDL_ShowCursor(SDL_ENABLE);
	SDL_FreeSurface(drawContext);
}

void SORE_Kernel::Renderer::Frame(int elapsedTime)
{
	SORE_Profiler::Sample graphics("graphics");
	static int frames = 0;
	static int T0 = SORE_Timing::GetGlobalTicks();
	static float fps;
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

bool SORE_Kernel::Renderer::OnResize(Event* event=NULL)
{
	GLint width, height;
	if(event==NULL)
	{
		glGetIntegerv(GL_VIEWPORT, viewport);
		width = viewport[2]-viewport[0];
		height = viewport[3]-viewport[1];
	}
	else
	{
		width  = event->resize.w;
		height = event->resize.h;
		drawContext = SDL_SetVideoMode(width, height, 0, videoFlags);
	}
	/* Height / width ration */
	GLfloat ratio;
	ratio = ( GLfloat )width / ( GLfloat )height;
	/* Setup our viewport. */
	glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );
	/* change to the projection matrix and set our viewing volume. */
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	/* Set our perspective */
	//gluPerspective( 45.0f, ratio, 0.1f, 300.0f );

	//gluOrtho2D(-x, x, y, -y);
	if(ChangeProjection(ratio)!=0)
		return false;
	/* Make sure we're chaning the model view and not the projection */
	glMatrixMode( GL_MODELVIEW );
	/* Reset The View */
	glLoadIdentity( );
	return true;
}

int SORE_Kernel::Renderer::ChangeProjection(double ratio)
{
	switch(proj.type)
	{
		case SORE_Graphics::NONE:
			ENGINE_LOG_S(SORE_Logging::LVL_ERROR, "No projection type set, could not initialize projection");
			return -1;
			break;
		case SORE_Graphics::ORTHO2D:
			if(proj.useScreenCoords)
				gluOrtho2D(0, viewport[2], 0, viewport[3]);
			else
			{
				if(proj.useScreenRatio)
				{
					proj.top = proj.left / ratio;
					proj.bottom = proj.right / ratio;
				}
				gluOrtho2D(proj.left, proj.right, proj.top, proj.bottom);
			}
			break;
		case SORE_Graphics::ORTHO:
			//TODO: finish ortho projection
			break;
		case SORE_Graphics::PERSPECTIVE:
			if(proj.useScreenRatio)
			{
				proj.ratio = ratio;
			}
			gluPerspective(proj.fov, proj.ratio, proj.znear, proj.zfar );
			break;
		default:
			return -1;
			break;
	}
	return 0;
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
		ENGINE_LOG(SORE_Logging::SHOW_CRITICAL, "Video query failed: %s", SDL_GetError());
		return 1;
	}
	//save current resolution
	width = videoInfo->current_w;
	height = videoInfo->current_h;

	videoFlags = SDL_OPENGL;
	if(screen.fullscreen)
		videoFlags |= SDL_FULLSCREEN;
	if(screen.showCursor)
		SDL_ShowCursor(SDL_ENABLE);
	else
		SDL_ShowCursor(SDL_DISABLE);
	if(screen.resizable)
		videoFlags |= SDL_RESIZABLE;
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
	
	//Uint8* mask = SORE_Utility::GetBMPMask("data/snake.bmp");
	SDL_Surface* icon = SDL_LoadBMP("data/snake.bmp");
	
	SDL_WM_SetIcon(icon, NULL);
	SDL_FreeSurface(icon);
	SDL_WM_SetCaption("SNAKE!", "SNAKE!");
	drawContext = SDL_SetVideoMode(screen.width, screen.height, 0, videoFlags);
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
	ENGINE_LOG(SORE_Logging::LVL_INFO, "OpenGL Rendering information\nRenderer   : %s\nVender     : %s\nAPI Version: %s",(char*)glGetString(GL_RENDERER),(char*)glGetString(GL_VENDOR),(char*)glGetString(GL_VERSION));
#ifdef WIN32
	if(WGLEW_wglSwapInterval)
	{
		wglSwapInterval(0); //turn off vsync
	}
	else
	{
		ENGINE_LOG_S(SORE_Logging::LVL_WARNING, "Vsync control not available");
	}
#endif
	return 0;
}

void SORE_Kernel::Renderer::SetProjection(SORE_Graphics::ProjectionInfo& info)
{
	proj = info;
	OnResize();
	info.top = proj.top;
	info.bottom = proj.bottom;
	info.ratio = proj.ratio;
}

void SORE_Kernel::Renderer::InitExtensions()
{
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK)
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, "Failed to initialize OpenGL extensions. GLEW Error: %s", glewGetErrorString(glewError));
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
