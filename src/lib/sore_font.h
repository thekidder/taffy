/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR        *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#ifndef  SORE_FONT_H
#define  SORE_FONT_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <sore_matrix4x4.h>
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

    class SORE_EXPORT Font : public SORE_Resource::Resource, SORE_Utility::Noncopyable
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
        bool libraryInit;
        FT_Face face;
        FT_Byte* fontInfo;
    };

    typedef boost::shared_ptr<Font> FontPtr;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
