
#ifndef  __SORE_UTIL_H__
#define  __SORE_UTIL_H__

#include "sore_allgl.h"
#include <cmath>

using namespace std;

namespace SORE_Utility
{
	int GetFileExt(const char* filename, char* ext); //returns 0 on success
	float getRandomMinMax( float fMin, float fMax );
	Uint8* GetBMPMask(const char* filename);
}

//Functions
bool  CheckExtension(const char* extensionName);

void  BuildFont();
void  glPrint(const char *fmt, ...);

const float PI_180 = (float)M_PI/180.0f;

//extern PFNGLPOINTPARAMETERFARBPROC  glPointParameterf;
//extern PFNGLPOINTPARAMETERFVARBPROC  glPointParameterfv;
//extern GLuint fontList;


#endif //__SORE_UTIL_H__
