/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// $Id$

#include "sore_font.h"
#include "sore_fileio.h"
#include "sore_logger.h"

#include "sore_geometrychunk.h"
#include "sore_texture.h"

#include <vector>
#include <string>
#include <string.h>
#include <cstdlib>

#include <boost/lexical_cast.hpp>

namespace SORE_Font
{
	//number of characters to load for each face
	//we'll load the full ASCII set
	const unsigned int NUM_CHARACTERS = 128;

	std::vector<std::string> FontPaths::fontPaths;

	inline int next_p2 (int a )
	{
		int rval=1;
		while(rval<a) rval<<=1;
		return rval;
	}

	Font::Font(std::string filename)
		: Resource(filename, "", true)
	{
		std::string path = FontPaths::GetFontPath(filename);
		SetFilename(path);

		Load();
	}

	Font::~Font()
	{
		delete[] fontInfo;
		for(std::map<unsigned int, CharInfo*>::iterator it = characters.begin();it!=characters.end();++it)
		{
			for(unsigned int i=0;i<NUM_CHARACTERS;++i)
			{
				if(it->second[i].tex)
					delete it->second[i].tex;
				if(it->second[i].gc)
					delete it->second[i].gc;
			}
			CharInfo* c = it->second;
			delete[] c;
		}
		FT_Done_Face(face);
		FT_Done_FreeType(library);
	}

	void Font::LoadFace(unsigned int height)
	{
		ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Loading face for %d px %s") % height % GetFilename());
		FT_Set_Pixel_Sizes( face, 0, height);
		
		characters[height] = new CharInfo[NUM_CHARACTERS];

		for(unsigned char i=0;i<128;i++)
			LoadCharacter(face,i, height);
	}

	void Font::Load()
	{
		characters.clear();
		if (FT_Init_FreeType( &library )) 
			return;
	
		SORE_FileIO::file_ref fontObj = SORE_FileIO::Open(GetFilename().c_str());
		
		size_t size = SORE_FileIO::Size(fontObj);
		size_t err;
	
		fontInfo = new FT_Byte[size];
	
		if((err=SORE_FileIO::Read(fontInfo, 1, size, fontObj))!=size)
    {
      ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format(
																												"Font load failed: Could not read font from disk "
																												"(expected %d bytes, read %d bytes)") % size % err);
      SORE_FileIO::Close(fontObj);
      return;
    }
		SORE_FileIO::Close(fontObj);
		if ((err=FT_New_Memory_Face( library, fontInfo, static_cast<FT_Long>(size), 0, &face ))!=0) 
    {
      FT_Done_FreeType(library);
      ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format(
																												"Font load failed: Freetype error code %d") % err);
      return;
    }
		return;
	}

	void Font::LoadCharacter(FT_Face& face, char ch, unsigned int h)
	{
		FT_Render_Mode mode = FT_RENDER_MODE_NORMAL;
		if(h <= 24)
			mode = FT_RENDER_MODE_MONO; //NORMAL gives odd glitches with small fonts

		if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), mode ))
			return;

		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		FT_Bitmap& bitmap=face->glyph->bitmap;
	
		int width = next_p2( bitmap.width );
		int height = next_p2( bitmap.rows );

		GLubyte* expanded_data = new GLubyte[ 2 * width * height];
	
		for(int j=0; j<height;j++)
    {
      for(int i=0; i<width; i++)
			{
				//set alpha to the FreeType bitmap result, and luminance to max
				expanded_data[2*(i+j*width)] = 255;
				expanded_data[2*(i+j*width)+1] = 
					(i>=bitmap.width || j>=bitmap.rows) ?
					0 : bitmap.buffer[i + bitmap.width*j];
			}
    }

		float x = static_cast<float>(bitmap.width) / static_cast<float>(width), 
			y = static_cast<float>(bitmap.rows) / static_cast<float>(height);

		CharInfo* c = characters[h];

		if(x != 0 && y != 0)
		{
			c[ch].tex = new SORE_Resource::Texture2D(expanded_data, GL_RGBA, 
																							 GL_LUMINANCE_ALPHA, width, height);

			c[ch].transform = SORE_Math::Matrix4<float>::GetTranslation(
																																	static_cast<float>(face->glyph->bitmap_left), 
																																	static_cast<float>(bitmap.rows-face->glyph->bitmap_top) + (h - bitmap.rows),
																																	0.0f);

			SORE_Math::Rect<float> bounds (0.0f, 
																		 static_cast<float>(bitmap.width), 0.0f, 
																		 static_cast<float>(bitmap.rows));
			SORE_Math::Rect<float> texCoords(0.0f, x, 0.0f, y);

			c[ch].gc = new SORE_Graphics::GeometryChunk(c[ch].tex, bounds, texCoords);
		}
		else
		{
			c[ch].tex = NULL;
			c[ch].gc = NULL;
		}
		c[ch].advance = static_cast<float>(face->glyph->advance.x >> 6);

		delete [] expanded_data;
	}

	const CharInfo& Font::GetCharacter(unsigned int height, char c)
	{
		if(c > 127 && c < 0)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Attempted to get non-existent character");
			return characters[height][0];
		}
		else
		{
			if(characters.find(height)==characters.end())
				LoadFace(height);
			return characters[height][c];
		}
	}

	std::string FontPaths::GetFontPath(std::string name)
	{
		if(fontPaths.size()==0) InitPaths();
	
		std::string full = name;
		SORE_FileIO::file_ref fontObj = SORE_FileIO::Open(full.c_str());
	
		if(fontObj==0)
		{
			std::vector<std::string>::iterator it;
			for(it=fontPaths.begin();it<fontPaths.end();it++)
			{
				full = *it;
				full += name;
				fontObj = SORE_FileIO::Open(full.c_str());
				if(fontObj!=0)
				{
					break;
				}
				fontObj = 0;
			}
		}
		if(fontObj)
		{
			SORE_FileIO::Close(fontObj);
			return full;
		}
		else
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Could not find font %s") % name);
			return "";
		}
	}
}
