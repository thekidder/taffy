// $Id$

#include "sore_font.h"

void SORE_Font::Font::InitPaths()
{
	fontPaths.clear();
	if(!FcInit())
	{
		return;
	}
	FcConfig* config = FcConfigGetCurrent();
	FcStrList* fontDirs = FcConfigGetFontDirs(config);
	
	char* dir;
	
	std::string path;
	
	while((dir = (char*)FcStrListNext(fontDirs)))
	{
		path = dir;
		path += "/";
		fontPaths.push_back(path);
	}
	FcStrListDone(fontDirs);
		return;
}
