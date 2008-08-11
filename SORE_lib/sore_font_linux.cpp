// $Id$

#include "sore_font.h"
#include <fontconfig.h>

void SORE_Font::Font::InitPaths()
{
	SORE_Font::fontPaths.clear();
	if(!FcInit())
	{
		return LIBRARY_LOAD_FAILED;
	}
	FcConfig* config = FcConfigGetCurrent();
	FcStrList* fontDirs = FcConfigGetFontDirs(config);
	
	char dirName[2048];

	char fontPath[2048];
	while((dirName = (char*)FcStrListNext(fontDirs)))
	{
		strcpy(fontPath, dirName);
		strcat(fontPath, "/");
		fontPaths.push_back(fontPath);
	}
	FcStrListDone(fontDirs);
		return 0;
}
