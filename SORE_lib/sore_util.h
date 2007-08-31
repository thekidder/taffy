
#ifndef  __SORE_UTIL_H__
#define  __SORE_UTIL_H__

#include "sore_allgl.h"

using namespace std;

namespace SORE_Utility
{
	int GetFileExt(const char* filename, char* ext); //returns 0 on success
}

//Functions
bool  CheckExtension(const char* extensionName);
float getRandomMinMax( float fMin, float fMax );
float Distance(float x1, float y1, float z1, float x2, float y2, float z2);
void  BuildFont();
void  glPrint(const char *fmt, ...);

const float PI_180 = (float)M_PI/180.0f;

//extern PFNGLPOINTPARAMETERFARBPROC  glPointParameterf;
//extern PFNGLPOINTPARAMETERFVARBPROC  glPointParameterfv;
//extern GLuint fontList;


#endif //__SORE_UTIL_H__
