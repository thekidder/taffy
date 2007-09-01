//
// C++ Interface: extensions
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef  __SORE_GLEXT_H__
#define  __SORE_GLEXT_H__

typedef int(*TESTPTR)();

extern TESTPTR test;

// OpenGL extension function pointers
#define GLEXT_PROC(proc, name) extern proc name
#include "glextproc.h"
#undef GLEXT_PROC 

#endif /*__SORE_GLEXT_H__*/
