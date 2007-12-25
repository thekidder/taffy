
#ifndef  __ALLGL_H__
#define  __ALLGL_H__

#ifdef _WIN32
#include <windows.h>
#endif

//#define GL_GLEXT_PROTOTYPES
//#undef GL_ARB_shader_objects

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#ifdef WIN32
	#include <SDL.h>
	
	//wglGetProcAddress
	#include <GL/wglext.h>
	#define uglGetProcAddress(x) wglGetProcAddress(x)
	
	//We need M_PI
#else /* GLX */
	#include <SDL/SDL.h>
	//#include <SDL/SDL_image.h>
	#include <SDL/SDL_opengl.h>
	#include <GL/glx.h>
	
	//Linux-specific to find system fonts
	#include <fontconfig/fontconfig.h>
	
	//glXGetProcAddress
	#define uglGetProcAddress(x) (*glXGetProcAddress)((const GLubyte*)(x))
#endif
#endif //__ALLGL_H__
