
#ifndef  __ALLGL_H__
#define  __ALLGL_H__

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef WIN32
	#include <SDL.h>
	#include <GL/wglew.h>
	//wglGetProcAddress
	
	//#define uglGetProcAddress(x) wglGetProcAddress(x)
	
	//We need M_PI
#else /* GLX */
	#include <SDL/SDL.h>
	//#include <SDL/SDL_image.h>
	//#include <SDL/SDL_opengl.h>
#include <GL/glxew.h>
	
	//Linux-specific to find system fonts
	#include <fontconfig/fontconfig.h>
	
	//glXGetProcAddress
	//#define uglGetProcAddress(x) (*glXGetProcAddress)((const GLubyte*)(x))
#endif
#endif //__ALLGL_H__
