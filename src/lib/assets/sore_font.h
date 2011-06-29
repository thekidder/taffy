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

#ifndef SORE_FONT_H
#define SORE_FONT_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <sore_allgl.h>
#include <sore_assettypes.h>
#include <sore_matrix4x4.h>
#include <sore_geometrychunk.h>
#include <sore_texturestate.h>

//freetype
#include <ft2build.h>
#include FT_FREETYPE_H

#include <boost/format.hpp>

namespace SORE_Resource
{
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
        // construct a renderable using the given vertices and texture
        SORE_Graphics::vertex vertices[4];
        SORE_Graphics::TextureState::TextureObject texture;
        float advance;
    };

    // NOT thread safe: each thread needs own instance of FT_library
    class SORE_EXPORT Font : SORE_Utility::Noncopyable
    {
    public:
        Font(FT_Byte* faceData_, size_t length);
        ~Font();

        void LoadFace(unsigned int height);
        const CharInfo& GetCharacter(unsigned int height, char c);
        float Width(unsigned int height, const std::string str);

        bool Loaded() const { return true; }
    private:
        struct CharInfoInternal
        {
            GLubyte* data;
            unsigned int height;
            unsigned int width;
            float x;
            float y;
        };

        void LoadCharacter(char ch, unsigned int h,
                           CharInfoInternal& info,
                           unsigned int& width,
                           unsigned int& height);

        //(height, CharInfo[128])
        std::map<unsigned int, CharInfo*> characters;
        std::map<unsigned int, SORE_Resource::Texture2DPtr> textures;
        static FT_Library library;
        FT_Face face;
        std::vector<FT_Byte> faceData;
        
        // used so we know when to clean up freetype
        static int numFonts;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
