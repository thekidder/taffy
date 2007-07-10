#ifndef  __FONT_H__
#define  __FONT_H__

#include "allgl.h"

//freetype
#include <ft2build.h>
#include FT_FREETYPE_H

namespace SORE_Font
{
	//let's define some nice error constants
	const int LIBRARY_LOAD_FAILED    = 1;
	const int FONTCONFIG_LOAD_FAILED = 5;
	const int FONT_LOAD_FAILED       = 2;
	const int GLYPH_LOAD_FAILED      = 3;
	const int GET_GLYPH_FAILED       = 4;
	const int INVALID_FONT_NAME      = 6;
	const int INVALID_FONT_OBJ       = 7;
	const int INVALID_FONT_HEIGHT    = 8;
	
	struct FontInfo
	{
		int      height;
		GLuint*  textures;
		GLuint   listBase;
		FT_Byte* fontInfo;
		
		int     Init(const char* fontName, unsigned int h);
		int     Init();
		int     LoadFont(const char* fontName);
		int     SetHeight(unsigned int h);
		void    Cleanup();
	};
	
	typedef int font_ref;
	
	
	int         Init();
	font_ref    LoadFont(const char* font, unsigned int h);
	int         FontHeight(font_ref font);
	
	int         MakeDisplayList( FT_Face face, char ch, GLuint list_base, GLuint * tex_base );
	inline void PushScreenCoordMatrix();
	inline void PopProjectionMatrix();
	
	int         Print(font_ref fontIndex, int x, int y, const char* fmt, ...);
	float       Print(font_ref fontIndex, int x, int y, char c);
	void        Cleanup();

}

#endif //__FONT_H__
