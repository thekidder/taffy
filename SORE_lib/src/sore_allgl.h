// $Id$

#ifndef  __ALLGL_H__
#define  __ALLGL_H__

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

#ifdef _WIN32
	#include <GL/wglew.h>
#else /* GLX */
	
#include <GL/glxew.h>
	//Linux-specific to find system fonts
	#include <fontconfig/fontconfig.h>
#endif
#endif //__ALLGL_H__
