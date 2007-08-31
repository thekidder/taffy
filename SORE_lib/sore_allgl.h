
#ifndef  __ALLGL_H__
#define  __ALLGL_H__

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef WIN32
	#include <SDL.h>
	//#include <SDL_image.h>
	
	//wglGetProcAddress
	#include <GL/wglext.h>
	#define uglGetProcAddress(x) wglGetProcAddress(x)
	
	//We need M_PI
	#define _USE_MATH_DEFINES
#else /* GLX */
	#include <GL/glext.h>
	#include <SDL/SDL.h>
	#include <SDL/SDL_image.h>
	#include <SDL/SDL_opengl.h>
	#include <GL/glx.h>
	
	//Linux-specific to find system fonts
	#include <fontconfig/fontconfig.h>
	
	//glXGetProcAddress
	#define uglGetProcAddress(x) (*glXGetProcAddress)((const GLubyte*)(x))
#endif

#include <iostream>
#include <cstdlib>
#include <cmath>

#endif //__ALLGL_H__
