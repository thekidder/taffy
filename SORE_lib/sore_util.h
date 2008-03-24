// $Id$

#ifndef  __SORE_UTIL_H__
#define  __SORE_UTIL_H__

#include <cmath>
#include <map>
#include <string>

using namespace std;

namespace SORE_Utility
{
	typedef std::map<std::string, std::map<std::string, std::string> > settings_map;
	int GetFileExt(const char* filename, char* ext); //returns 0 on success
	double getRandomMinMax( double dMin, double fMax );
	float getRandomMinMax( float fMin, float fMax );
	settings_map ParseIniFile(const char* file);
	std::string TrimString(std::string toTrim);
}

#endif //__SORE_UTIL_H__
