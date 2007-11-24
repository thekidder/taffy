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
#include "sore_logger.h"
#include "sore_profiler.h"
#include "sore_graphics.h"
#include "sore_util.h"
#include <cassert>

SORE_Kernel::Renderer::Renderer()
{
	if(InitializeSDL()!=0)
	{
		ENGINE_LOG(SORE_Logging::LVL_CRITICAL, "Could not initialize SDL (SDL error %s)", SDL_GetError());
	}
	if(InitializeGL()!=0)
	{
		ENGINE_LOG_S(SORE_Logging::LVL_CRITICAL, "Could not initialize GL");
	}
	if(InitializeSOREGraphics()!=0)
	{
		ENGINE_LOG_S(SORE_Logging::LVL_CRITICAL, "Could not initialize SORE Graphics subsystems");
	}
	font = SORE_Font::LoadFont("data/Fonts/liberationmono.ttf", 24);
	if(font == 0)
	{
		ENGINE_LOG_S(SORE_Logging::LVL_ERROR, "Could not load renderer font");
		GameKernel* gk = GameKernel::GetKernel();
		gk->quitFlag = true;
	}
	sg = NULL;
	cam = NULL;
}

SORE_Kernel::Renderer::~Renderer()
{
}

void SORE_Kernel::Renderer::Frame(int elapsedTime)
{
	SORE_Profiler::Sample graphics("graphics");
	static int frames = 0;
	static int T0 = SDL_GetTicks();
	static float fps;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//assert(sg!=NULL && "No scene graph");
	if(sg)
	{
		if(cam)
			cam->TransformView();
		sg->Render();
	}
	SORE_Graphics::Init_2DCanvas();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//SORE_Graphics::DrawString(font, 0, 0, "FPS: %5.2f", 1000.0/float(elapsedTime));
	
	frames++;
	{
		GLint t = SDL_GetTicks();
		if (t - T0 >= 400) //display FPS every 400 milliseconds
		{
			GLfloat seconds = (GLfloat)((t - T0) / 1000.0);
			fps = frames / seconds;
			T0 = t;
			frames = 0;
		}
	}
	
	SORE_Graphics::DrawString(font, 0, 0, "FPS: %5.2f", fps);
	
	SORE_Graphics::DrawString(font, 0, 36,  "Controls");
	SORE_Graphics::DrawString(font, 0, 64,  "m - grab mouse         u - ungrab mouse");
	SORE_Graphics::DrawString(font, 0, 92,  "l - toggle lighting    n - toggle normals");
	SORE_Graphics::DrawString(font, 0, 120, "x - toggle wireframe   c - toggle coloring");
	
	SORE_Graphics::Destroy_2DCanvas();
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
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		width = viewport[2];
		height = viewport[3];
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
	gluPerspective( 45.0f, ratio, 0.1f, 300.0f );
	/* Make sure we're chaning the model view and not the projection */
	glMatrixMode( GL_MODELVIEW );
	/* Reset The View */
	glLoadIdentity( );
	return true;
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

	videoFlags = SDL_OPENGL;
	//if(FULLSCREEN) videoFlags |= SDL_FULLSCREEN;
	//if(RESIZEABLE) videoFlags |= SDL_RESIZABLE;
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
	drawContext = SDL_SetVideoMode(800, 600, 0, videoFlags);
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
	OnResize();
	SORE_Graphics::InitExtensions();
	ENGINE_LOG(SORE_Logging::LVL_INFO, "OpenGL Rendering information\nRenderer   : %s\nVender     : %s\nAPI Version: %s",(char*)glGetString(GL_RENDERER),(char*)glGetString(GL_VENDOR),(char*)glGetString(GL_VERSION));
	if(wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT(0); //turn off vsync
	}
	else
	{
		ENGINE_LOG_S(SORE_Logging::LVL_WARNING, "Vsync control not available");
	}
	return 0;
}

int SORE_Kernel::Renderer::InitializeSOREGraphics()
{
	SORE_Graphics::Init2DOverlay();
	return 0;
}
