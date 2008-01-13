// $Id$

#ifndef  __SORE_UTIL_H__
#define  __SORE_UTIL_H__

#include "sore_allgl.h"
#include "sore_defines.h"
#include <cmath>

using namespace std;

namespace SORE_Utility
{
	int GetFileExt(const char* filename, char* ext); //returns 0 on success
	float getRandomMinMax( float fMin, float fMax );
}

#endif //__SORE_UTIL_H__
