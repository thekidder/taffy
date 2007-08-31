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

// OpenGL extension function pointers
#define GLEXT_PROC(proc, name) extern proc name
#include "glextproc.h"
#undef GLEXT_PROC 
