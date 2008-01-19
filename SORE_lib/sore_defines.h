// $Id$

#ifndef  __SORE_DEFINES_H__
#define  __SORE_DEFINES_H__

#ifdef NDEBUG
#undef DEBUG
#define DEBUG 1
#else
#define DEBUG 1
#endif

#ifdef _WIN32
//We need M_PI
#include <windows.h>
#define _USE_MATH_DEFINES
#else /* GLX */
#endif

#endif /*__SORE_DEFINES_H__*/
