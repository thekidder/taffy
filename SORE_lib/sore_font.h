// $Id$

#ifndef  __FONT_H__
#define  __FONT_H__

#include "sore_allgl.h"
#include "sore_resource.h"

//freetype
#include <ft2build.h>
#include FT_FREETYPE_H

#include <boost/format.hpp>

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
	
	class Font : public SORE_Resource::Resource
	{
		public:
			//pass height in pixels as a string
			Font(std::string filename, std::string fHeight);
			~Font();
			
			const char* Type() {return "Font";}
			unsigned int Height() const;
			
			float Print(int x, int y, std::string str);
			float Print(int x, int y, boost::format str);
		protected:
			void Load();
		private:
			void MakeDisplayList(FT_Face& face, char ch);
			
			static void InitPaths();
			static std::vector<std::string> fontPaths;
			
			unsigned int height;
			
			GLuint*  textures;
			GLuint   listBase;
	};

	/*int         Print(font_ref fontIndex, int x, int y, const char* fmt, ...);
	float       Print(font_ref fontIndex, int x, int y, char c);*/

}

#endif //__FONT_H__
