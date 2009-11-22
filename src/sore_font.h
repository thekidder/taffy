/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef  SORE_FONT_H
#define  SORE_FONT_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include "sore_allgl.h"
#include "sore_geometrychunk.h"
#include "sore_resource.h"
#include "sore_texture.h"

#include "math/sore_matrix4x4.h"

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

    class SORE_EXPORT FontPaths
    {
    public:
        static void InitPaths();
        static std::string GetFontPath(std::string name, SORE_FileIO::PackageCache* pc = NULL);
    private:
        static std::vector<std::string> fontPaths;
    };

    struct CharInfo
    {
        SORE_Graphics::Texture2D* tex;
        SORE_Graphics::GeometryChunk* gc;
        SORE_Math::Matrix4<float> transform;
        float advance;
    };

    class SORE_EXPORT Font : public SORE_Resource::Resource
    {
    public:
        Font(std::string filename, SORE_FileIO::PackageCache* pc);
        ~Font();

        const char* Type() {return "Font";}
        bool GLContextDependent() const {return true;}

        void LoadFace(unsigned int height);
        const CharInfo& GetCharacter(unsigned int height, char c);
    protected:
        //This loads our face, but no specific characters
        void Load();
    private:
        void LoadCharacter(FT_Face& face, char ch, unsigned int h);

        //(height, CharInfo[128])
        std::map<unsigned int, CharInfo*> characters;
        FT_Library library;
        FT_Face face;
        FT_Byte* fontInfo;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
