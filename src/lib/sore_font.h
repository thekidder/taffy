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

#include "math/sore_matrix4x4.h"
#include <sore_allgl.h>
#include <sore_renderable.h>
#include <sore_resource.h>
#include <sore_texture.h>

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
        static std::string GetFontPath(const std::string& name);
    private:
        static void InitPaths();

        static std::vector<std::string> fontPaths;
    };

    struct CharInfo
    {
        SORE_Graphics::Renderable renderable;
        float advance;
    };

    struct CharInfoInternal
    {
        GLubyte* data;
        unsigned int height;
        unsigned int width;
        SORE_Graphics::TransformationPtr transform;
    };

    class SORE_EXPORT Font : public SORE_Resource::Resource
    {
    public:
        Font(SORE_Resource::WatchedFileArrayPtr wfa);
        ~Font();

        const char* Type() {return "Font";}
        bool GLContextDependent() const {return true;}

        void LoadFace(unsigned int height);
        const CharInfo& GetCharacter(unsigned int height, char c);

        static std::string ProcessFilename(const std::string& filename);
    protected:
        //This loads our face, but no specific characters
        virtual void Load();
    private:
        Font(const Font& o);
        Font& operator=(const Font& o);
        void LoadCharacter(char ch, unsigned int h,
                           CharInfoInternal& info,
                           unsigned int& width,
                           unsigned int& height);

        //(height, CharInfo[128])
        std::map<unsigned int, CharInfo*> characters;
        std::map<unsigned int, SORE_Graphics::Texture2DPtr> textures;
        FT_Library library;
        FT_Face face;
        FT_Byte* fontInfo;
    };

    typedef boost::shared_ptr<Font> FontPtr;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
