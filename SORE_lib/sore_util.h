// $Id$

#ifndef  __SORE_UTIL_H__
#define  __SORE_UTIL_H__

#include "sore_allgl.h"
#include "sore_defines.h"
#include <cmath>
#include <map>
#include <string>

using namespace std;

namespace SORE_Utility
{
	int GetFileExt(const char* filename, char* ext); //returns 0 on success
	float getRandomMinMax( float fMin, float fMax );
	std::map<std::string, std::map<std::string, std::string> > ParseIniFile(const char* file);
	std::string TrimString(std::string toTrim);
}

#endif //__SORE_UTIL_H__
