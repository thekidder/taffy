
#ifndef  __SORE_DEFINES_H__
#define  __SORE_DEFINES_H__

#ifndef NDEBUG
#define DEBUG 1
#else
#undef DEBUG
#endif


#ifdef WIN32
	//We need M_PI
#define _USE_MATH_DEFINES
#else /* GLX */
#endif

#endif /*__SORE_DEFINES_H__*/
