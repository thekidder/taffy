
#include "main.h"

PFNGLPOINTPARAMETERFARBPROC  glPointParameterf  = NULL;
PFNGLPOINTPARAMETERFVARBPROC  glPointParameterfv  = NULL;

//Constants
static const int   WIDTH       = 1200;
static const int   HEIGHT      = 800;
static const bool  FULLSCREEN  = false;
static const bool  RESIZEABLE  = true;
static const char* TITLE       = "SDL OpenGL App";
static const GLfloat MOUSE_SENS = 5.0f;

static const int RUN_GAME_LOOP = 1;

//Globals
bool keys[512];
bool  isActive;
bool grabbed;
int mouseX = WIDTH/2;
int mouseY = HEIGHT/2;
SDL_Surface* drawContext;
Uint32 videoFlags;
GLfloat xrot, yrot, zrot;
GLfloat rotConst = 0.8f;
GLuint texture[4];
GLfloat LightAmbient[]=  {  0.01f, 0.01f, 0.01f, 1.0f };
GLfloat LightDiffuse[]=  {  0.6f, 0.6f, 0.6f, 1.0f };
GLfloat LightPosition[]= { -3.0f, 0.0f, 0.0f, 1.0f };
Camera camera;
GLuint fontList;
ParticleEngine particles;
SORE_Font::font_ref font;
GLfloat fps;
SORE_Util::Console console;
float r, g, b;
GLfloat circlePos[3];
SORE_Logging::Logger* mainLog;
SORE_Logging::FileLogger* fileLog;
SORE_Logging::ConsoleLogger* consoleLog;

double angle;

int main(int argc, char *argv[])
{
	SORE_Logging::InitLogging();
	mainLog = new SORE_Logging::Logger;
	fileLog = new SORE_Logging::FileLogger(SORE_Logging::INFO, "program.log");
	consoleLog = new SORE_Logging::ConsoleLogger(SORE_Logging::INFO);
	mainLog->AddBackend(fileLog);
	mainLog->AddBackend(consoleLog);
	mainLog->Flush();
#ifdef DEBUG
	std::cout << "----DEBUG BUILD----\n";
#endif
	SORE_FileIO::Init();
	SORE_FileIO::CachePackageInfo("data_compressed.sdp");
	//SORE_FileIO::CachePackageInfo("data.sdp");
	
	if(InitSDL()!=0)
		return -1;
	
	if(InitGL()!=0)
		return -1;
	
	
	
	SORE_Font::Init();
	font = SORE_Font::LoadFont("data/Fonts/liberationsans.ttf", 24);	
	console.InitFont(SORE_Font::LoadFont("data/Fonts/liberationmono.ttf", 24));
	
	SORE_Graphics::Init_2DOverlay();
	
	if(LoadGLTextures()!=0)
	{
		std::cerr << "Failed loading texture\n";
		return -1;
	}
	Resize(WIDTH, HEIGHT);
	
	particles.SetTexture(texture[1]);
	particles.SetPosition(3.0f, 0.0f, 0.0f);
	
	GLfloat fireColors[] = {0.6f, 1.0f, 0.1f, 0.4f, 0.0f, 0.3f};
	
	particles.SetColorRange(fireColors);
	particles.UsePointSpriteMode(true);
	particles.Initialize();
	
	return GameLoop();
}

int InitSDL()
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)!=0)
	{
		std::cerr << "Unable to initialize SDL:" << SDL_GetError() << std::endl;
		return -1;
	}
	atexit(Cleanup); //clean up
	
	
	const SDL_VideoInfo *videoInfo;
	/* Fetch the video info */
	videoInfo = SDL_GetVideoInfo( );

	if ( !videoInfo )
	{
		std::cerr << "Video query failed: " << SDL_GetError() << std::endl;
		return -1;
	}

	videoFlags = SDL_OPENGL;
	if(FULLSCREEN) videoFlags |= SDL_FULLSCREEN;
	if(RESIZEABLE) videoFlags |= SDL_RESIZABLE;
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
	drawContext = SDL_SetVideoMode(WIDTH, HEIGHT, 0, videoFlags);
	SDL_WarpMouse(0, 0);
	
	SDL_WM_SetCaption(TITLE, TITLE);
	SDL_WM_SetIcon(SDL_LoadBMP("data/defaulticon.bmp"), 0);
	
	grabbed = false;
	
	//SDL_WM_GrabInput(SDL_GRAB_ON);
	//SDL_ShowCursor (SDL_DISABLE);
	
	
	return 0;
}

int InitGL()
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
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT ) ;
	glEnable ( GL_COLOR_MATERIAL ) ;
		
	//glEnable(GL_CULL_FACE);
	
	camera.Translate(-2.0f, 0.0f, 1.0f);
	camera.SetRotationUpperLimit(AXIS_X,  90.0f);
	camera.SetRotationLowerLimit(AXIS_X, -90.0f);
	
	isActive = true;
	
	//check for extensions
		
	if(CheckExtension("GL_ARB_point_sprite"))
	{
		std::cout << "GL_ARB_point_sprite extension found..." << std::endl;
		glPointParameterf  = (PFNGLPOINTPARAMETERFEXTPROC)uglGetProcAddress("glPointParameterf");
		glPointParameterfv  = (PFNGLPOINTPARAMETERFVEXTPROC)uglGetProcAddress("glPointParameterfv");
	}
	else
	{
		std::cout << "GL_ARB_point_sprite extension missing, now exiting." << std::endl;
		return 1;
	}
		

	
	r = g = angle = 0.0f;
		
	circlePos[0] = 1.0f;
	circlePos[1] = 2.0f;
	circlePos[2] = 1.0f;
	
	b = 0.7f;
	
	std::cout << "Testing ResourceManager...\n";
	
	SORE_Resource::ResourceManager* rm = SORE_Resource::ResourceManager::GetManager();
	rm->RegisterDataLoader((SORE_Resource::RES_LOAD_DATA)SORE_Resource::LoadDataBuffer, "buf");
	rm->RegisterLoader((SORE_Resource::RES_LOAD)SORE_Resource::LoadTexture, "tga");
	SORE_Resource::res_handle buf = rm->Register("\0", 10, "buf");
	rm->Register("data/Textures/ring.tga");
	SORE_Resource::Resource* buffer = rm->GetPtr(buf);
	char* data = dynamic_cast<SORE_Resource::ResourceData*>(buffer)->GetDataPtr();
	data[0] = 'a';
	data = dynamic_cast<SORE_Resource::ResourceData*>(rm->GetPtr(buf))->GetDataPtr();
	std::cout << "Testing....(result should be 'a'): " << data[0] << "\n";
	rm->Unregister(buf);
	return 0;
}

bool EventLoop()
{
	SDL_Event event;
	
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_ACTIVEEVENT:
				//std::cout << "s: " << (int)event.active.state << std::endl;
				//std::cout << "  g: " << (int)event.active.gain << std::endl;
				//if(event.active.gain == 0 && (int)event.active.state == 6) //6 seems to occur when window is minimized
				if(event.active.gain == 0)
				{	
					isActive = false;
				}
				if(event.active.gain == 1)
				{
					isActive = true;
				}
				break;
			case SDL_USEREVENT:
				HandleUserEvents(&event);
				break;
			case SDL_KEYDOWN:
				if(HandleKeydown(&event))
					return true;
				break;
			case SDL_KEYUP:
				HandleKeyup(&event);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEMOTION:
				HandleMouse(&event);
				break;
			case SDL_VIDEORESIZE:
				drawContext = SDL_SetVideoMode(event.resize.w, event.resize.h, 0, videoFlags);
				if(!drawContext)
				{
					std::cerr << "Could not get surface after resize\n";
					return true;
				}
				Resize(event.resize.w, event.resize.h);
				break;
			case SDL_QUIT:
				return true; //quit
				break;
		}
	}
	return false; //don't quit
}

void Resize(int width, int height)
{
	std::cout << width << ":" << height << std::endl;
	/* Height / width ration */
	GLfloat ratio;

	ratio = ( GLfloat )width / ( GLfloat )height;

	/* Setup our viewport. */
	glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

	/* change to the projection matrix and set our viewing volume. */
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	/* Set our perspective */
	gluPerspective( 45.0f, ratio, 0.1f, 100.0f );

	/* Make sure we're chaning the model view and not the projection */
	glMatrixMode( GL_MODELVIEW );

	/* Reset The View */
	glLoadIdentity( );
	
	mouseX = width/2;
	mouseY = height/2;
}

int Render()
{
	SORE_Resource::ResourceManager* rm = SORE_Resource::ResourceManager::GetManager();
	SORE_Resource::Resource* re;
	SORE_Resource::ResourceHandle* rd;
	/* These are to calculate our fps */
	static GLint T0     = 0;
	static GLint frames = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	camera.TransformView();
	
	glPushMatrix();
	
	//glLoadIdentity();
	//glTranslatef(circlePos[0], circlePos[1], circlePos[2]);
	
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable( GL_COLOR_MATERIAL ) ;
	glTranslatef(-3.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
		
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glVertex3f(-0.1f, -0.1f,  0.1f); //bottom
	glVertex3f( 0.1f, -0.1f,  0.1f);
	glVertex3f( 0.1f, -0.1f, -0.1f);
	glVertex3f(-0.1f, -0.1f, -0.1f);
	
	glEnd();
	glBegin(GL_LINE_LOOP);
		
	glVertex3f(-0.1f,  0.1f,  0.1f); //top
	glVertex3f( 0.1f,  0.1f,  0.1f);
	glVertex3f( 0.1f,  0.1f, -0.1f);
	glVertex3f(-0.1f,  0.1f, -0.1f);
		
	glEnd();
	glBegin(GL_LINE_LOOP);
	
		//front
	glVertex3f(-0.1f, -0.1f,  0.1f); //bottom left
	glVertex3f(-0.1f,  0.1f,  0.1f); //top left
	glVertex3f( 0.1f,  0.1f,  0.1f); //top right
	glVertex3f( 0.1f, -0.1f,  0.1f); //bottom right
	
	glEnd();
	glBegin(GL_LINE_LOOP);
		
	glVertex3f(-0.1f, -0.1f, -0.1f); //back
	glVertex3f(-0.1f,  0.1f, -0.1f);
	glVertex3f( 0.1f,  0.1f, -0.1f);
	glVertex3f( 0.1f, -0.1f, -0.1f);
	
	glEnd();
	glBegin(GL_LINE_LOOP);
			
	glVertex3f(-0.1f, -0.1f,  0.1f); //left
	glVertex3f(-0.1f,  0.1f,  0.1f);
	glVertex3f(-0.1f,  0.1f, -0.1f);
	glVertex3f(-0.1f, -0.1f, -0.1f);
	
	glEnd();
	glBegin(GL_LINE_LOOP);
		
	glVertex3f( 0.1f, -0.1f,  0.1f); //right
	glVertex3f( 0.1f,  0.1f,  0.1f);
	glVertex3f( 0.1f,  0.1f, -0.1f);
	glVertex3f( 0.1f, -0.1f, -0.1f);
		
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable ( GL_COLOR_MATERIAL ) ;
	glPopMatrix();

	//glRotatef( xrot, 1.0f, 0.0f, 0.0f); /* Rotate On The X Axis */
	//glRotatef( yrot, 0.0f, 1.0f, 0.0f); /* Rotate On The Y Axis */
	//glRotatef( zrot, 0.0f, 0.0f, 1.0f); /* Rotate On The Z Axis */
	
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);

    /* NOTE:
	*   The x coordinates of the glTexCoord2f function need to inverted
	* for SDL because of the way SDL_LoadBmp loads the data. So where
	* in the tutorial it has glTexCoord2f( 1.0f, 0.0f ); it should
	* now read glTexCoord2f( 0.0f, 0.0f );
		*/
	
	
	//glTranslatef( -3.0f, 0.0f, -8.0f);
	//glRotatef( xrot, 1.0f, 0.0f, 0.0f); /* Rotate On The X Axis */
	//glRotatef( yrot, 0.0f, 1.0f, 0.0f); /* Rotate On The Y Axis */
	//glRotatef( zrot, 0.0f, 0.0f, 1.0f); /* Rotate On The Z Axis */
	glDisable(GL_TEXTURE_2D);
	//glDisable(GL_LIGHTING);
	//glLineWidth(5.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f); //bottom 1
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glColor4f(1.0f,0.0f,0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glColor4f(0.0f,0.0f,1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
		
	glColor4f(0.0f,0.0f,1.0f, 1.0f); //bottom 2
	glVertex3f(1.0f, -1.0f, -1.0f);
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glColor4f(0.0f,1.0f,1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
		
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor4f(1.0f,0.0f,0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
		
	glColor4f(0.0f,1.0f,1.0f, 1.0f);

	glVertex3f(-1.0f, -1.0f, -1.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
		
	glColor4f(1.0f,0.0f,0.0f, 1.0f);
		
	glVertex3f(1.0f, -1.0f, 1.0f);
	glColor4f(0.0f,1.0f,0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor4f(0.0f,0.0f,1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
		
	glColor4f(0.0f,1.0f,1.0f, 1.0f);
		
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor4f(0.0f,0.0f,1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd( );
	//glLoadIdentity();
	
	glTranslatef(-5.0f, 0.0f, 0.0f);
	//glRotatef( xrot, 1.0f, 0.0f, 0.0f); 
	//glRotatef( yrot, 0.0f, 1.0f, 0.0f); 
	//glRotatef( zrot, 0.0f, 0.0f, 1.0f); 
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	re = rm->GetPtr("data/Textures/crate.tga");
	rd = dynamic_cast<SORE_Resource::ResourceHandle*>(re);
	glBindTexture( GL_TEXTURE_2D, rd->GetHandle());
	glColor4f(1.0f,1.0f,1.0, 1.0f);
	glBegin(GL_QUADS);
		
		
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f,  1.0f); //bottom
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		
		
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f); //top
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		
		//front
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f,  1.0f); //bottom left
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f); //top left
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f); //top right
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, -1.0f,  1.0f); //bottom right
		
		
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f); //back
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		
		
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f,  1.0f); //left
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		
		
	glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f,  1.0f); //right
	glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		
	glEnd();
	
	const static float one_50 = 1.0f*50.0f;
	
	re = rm->GetPtr("data/Textures/grass.tga");
	rd = dynamic_cast<SORE_Resource::ResourceHandle*>(re);
	glBindTexture( GL_TEXTURE_2D, rd->GetHandle());
	glBegin(GL_QUADS);
		
	glTexCoord2f(0.0f, one_50); glVertex3f(-50.0f, -2.0f, -50.0f); //right
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-50.0f, -2.0f,  50.0f);
	glTexCoord2f(one_50, 0.0f); glVertex3f( 50.0f, -2.0f,  50.0f);
	glTexCoord2f(one_50, one_50); glVertex3f( 50.0f, -2.0f, -50.0f);
		
	glEnd();
	
	particles.Display();
	
	
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDepthMask (GL_FALSE);
	
	re = rm->GetPtr("data/Textures/ring.tga");
	rd = dynamic_cast<SORE_Resource::ResourceHandle*>(re);
	glBindTexture( GL_TEXTURE_2D, rd->GetHandle());
	glColor4f(r, g, 1.0f, b);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(circlePos[0], circlePos[1], circlePos[2]);
	glBegin(GL_QUADS);
		
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -1.99f, -0.5f); //right
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -1.99f,  0.5f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -1.99f,  0.5f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f, -1.99f, -0.5f);
		
	glEnd();
	
	glDisable(GL_BLEND);
	glDepthMask (GL_TRUE);

	
	
	//glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	//glRasterPos2f(0.0f, 0.0f);
	//glPrint("Testing print");

	frames++;
	{
		GLint t = SDL_GetTicks();
		if (t - T0 >= 100) //display FPS every 100 milliseconds
		{
			GLfloat seconds = (GLfloat)((t - T0) / 1000.0);
			fps = frames / seconds;
			//std::cout << fps << "FPS: " << frames << " frames in " << seconds << " seconds\n";
			T0 = t;
			frames = 0;
		}
	}
	SORE_Graphics::Init_2DCanvas();
	console.Render();
	//SORE_Font::Print(font, 0, 0, "FPS: %5.2f", fps);
	SORE_Graphics::DrawString(font, 0, 0, "FPS: %5.2f", fps);
	SORE_Graphics::Destroy_2DCanvas();
	SDL_GL_SwapBuffers();
	return 0;
}


int HandleUserEvents(SDL_Event* event)
{
	switch(event->user.code)
	{
		case RUN_GAME_LOOP:
			Render();
			break;
	}
	return 0;
}

int HandleKeydown(SDL_Event* event)
{
	static bool r = true;
	keys[event->key.keysym.sym] = true;
	if(event->key.keysym.sym >= SDLK_a && event->key.keysym.sym <= SDLK_z)
	{
		console.PutChar((char)event->key.keysym.sym);
	}
	else if(event->key.keysym.sym == SDLK_RETURN)
	{
		console.PutChar('\n');
	}
	switch(event->key.keysym.sym)
	{
		case SDLK_F1:
			SDL_WM_ToggleFullScreen( drawContext );
			break;
		case SDLK_PLUS:
		case SDLK_EQUALS:
			rotConst *= 2.0f;
			break;
		case SDLK_MINUS:
			rotConst /= 2.0f;
			break;
		case SDLK_ESCAPE:
			ToggleGrab();
			break;
		case SDLK_r:
			r = !r;
			particles.UsePointSpriteMode(r);
			break;
	}
	return false; //continue execution;
}

int HandleKeyup(SDL_Event* event)
{
	keys[event->key.keysym.sym] = false;
	return 0;
}

int HandleMouse(SDL_Event* event)
{
	if(event->type == SDL_MOUSEMOTION && grabbed)
	{
		GLfloat y = event->motion.yrel/(GLfloat)MOUSE_SENS;
		GLfloat x = event->motion.xrel/(GLfloat)MOUSE_SENS;
		
		camera.Rotate(-y, 0.0f, 0.0f);
		camera.Rotate(0.0f, -x, 0.0f);
		
	}
	return 0;
}

void GameLogic(float timeElapsed, float timestep)
{
	static float bAdd = 0.03;
	GLfloat* rotation = camera.GetRotation();
	GLfloat yr = rotation[1];
	GLfloat xr = rotation[0];
	delete[] rotation;
	
	GLfloat x_rad, y_rad;
	x_rad = xr*M_PI/180.0f;
	y_rad = yr*M_PI/180.0f;
	
	if(keys[SDLK_w])
	{
		camera.Translate((GLfloat)(-0.1f * sin(y_rad)*abs(cos(x_rad))), (GLfloat)(0.1f * sin(x_rad)), (GLfloat)(-0.1f * cos(y_rad)*abs(cos(x_rad))));
	}
	if(keys[SDLK_s])
	{
		camera.Translate((GLfloat)(0.1f * sin(y_rad)*abs(cos(x_rad))), (GLfloat)(-0.1f * sin(x_rad)), (GLfloat)(0.1f * cos(y_rad)*abs(cos(x_rad))));
	}
	if(keys[SDLK_a])
	{
		camera.Translate((GLfloat)(0.1f * sin((yr-90.0)*PI_180)), (GLfloat)(0.0f), (GLfloat)(0.1f * cos((yr-90.0)*PI_180)));
	}
	if(keys[SDLK_d])
	{
		camera.Translate((GLfloat)(0.1f * sin((yr+90.0)*PI_180)), (GLfloat)(0.0f), (GLfloat)(0.1f * cos((yr+90.0)*PI_180)));
	}
	particles.Update();
	
	circlePos[0] += 0.02f * sin(angle);
	circlePos[2] += 0.02f * cos(angle);
	
	angle += 0.01;
	
	if(b>1.0f || b<0.2f) bAdd = -bAdd;
	b+=bAdd;
	//std::cout << "b: " << b << " bAdd: " << bAdd << "\n";
}

int GameLoop()
{
	const float logicFPS = 50.0;
	float accumulator = 0.0f;
	int lastTicks = 0;
	float totalTime = 0.0f;
	
	const float timestep = (float)(1000.0 / logicFPS);
	
	int lastFrameTime = 0;
	int renderTime = 0;
	
	bool done;
	
	done = false;
	
	while(!done)
	{	
		int ticks = SDL_GetTicks();
		int deltaTime = ticks - lastTicks;
		lastTicks = ticks;
		accumulator += deltaTime;
		
		while(accumulator>=timestep)
		{
			GameLogic(totalTime, timestep);
			totalTime += timestep;
			accumulator -= timestep;
		}
		
		done = EventLoop();
		if(isActive)
			Render();
		else
			SDL_Delay(1);
	}
	return 0;
}

int LoadGLTextures( )
{
	int status = -1;
	
	SORE_Resource::ResourceManager* rm = SORE_Resource::ResourceManager::GetManager();
	rm->Register("data/Textures/crate.tga");
	rm->Register("data/Textures/sprite.tga");
	rm->Register("data/Textures/ring.tga");
	rm->Register("data/Textures/grass.tga");
	
	//texture[0] = SORE_Texture::LoadTGA("data/Textures/crate.tga");
	//texture[1] = SORE_Texture::LoadTGA("data/Textures/sprite.tga");
	//texture[2] = SORE_Texture::LoadTGA("data/Textures/ring.tga");
	//texture[3] = SORE_Texture::LoadTGA("data/Textures/grass.tga");
		
	if(texture[0]!=-1 && texture[1]!=-1 && texture[2]!=-1 && texture[3]!=-1)
		status = 0;
	
	return status;
}

void Cleanup()
{
	bool success;
	success = true;
	SDL_Quit();
	if(!success)
	{
		std::cerr << "Failed to terminate successfully:" << SDL_GetError() << std::endl;
	} 
	glDeleteLists(fontList, 96);
	SORE_Font::Cleanup();
	delete fileLog;
	delete mainLog;
	delete consoleLog;
}

void ToggleGrab()
{
	if(grabbed)
	{
		SDL_WM_GrabInput(SDL_GRAB_OFF);
		SDL_ShowCursor(SDL_ENABLE);
	}
	else
	{
		SDL_WM_GrabInput(SDL_GRAB_ON);
		SDL_ShowCursor(SDL_DISABLE);
	}
	grabbed = !grabbed;
}


