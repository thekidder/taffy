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
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
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

#include <sore_fileio.h>
#include <sore_font.h>
#include <sore_geometry.h>
#include <sore_geometrychunk.h>
#include <sore_logger.h>
#include <sore_glslshader.h>
#include <sore_sprite.h>
#include <sore_texture.h>

#include <vector>
#include <string>
#include <string.h>
#include <cstdlib>
#include <fstream>

#include <boost/lexical_cast.hpp>

//number of characters to load for each face
//we'll load the full ASCII set
const static unsigned int NUM_CHARACTERS = 128;

std::vector<std::string> SORE_Font::FontPaths::fontPaths;

inline static int next_p2 (int a )
{
    int rval=1;
    while(rval<a) rval<<=1;
    return rval;
}

SORE_Font::Font::Font(SORE_Resource::WatchedFileArrayPtr wfa)
    : Resource(wfa)
{
    Load();
}

SORE_Font::Font::~Font()
{
    delete[] fontInfo;
    std::map<unsigned int, CharInfo*>::iterator it;
    for(it = characters.begin();it!=characters.end();++it)
    {
        CharInfo* c = it->second;
        delete[] c;
    }
    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

void SORE_Font::Font::Load()
{
    characters.clear();
    if (FT_Init_FreeType( &library ))
        return;

    SORE_FileIO::InFile* fontObj = File();

    size_t size = fontObj->size();
    size_t err;

    fontInfo = new FT_Byte[size];

    fontObj->strm().read(reinterpret_cast<char*>(fontInfo), size);
    err = fontObj->strm().gcount();
    delete fontObj;

    if(err != size)
    {
        ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format(
                       "Font load failed: Could not read font from disk "
                       "(expected %d bytes, read %d bytes)") % size % err);
        return;
    }

    if ((err=FT_New_Memory_Face( library, fontInfo, static_cast<FT_Long>(size),
                                 0, &face ))!=0)
    {
        FT_Done_FreeType(library);
        ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format(
                       "Font load failed: Freetype error code %d") % err);
        return;
    }
    return;
}

std::string SORE_Font::Font::ProcessFilename(const std::string& filename)
{
    return FontPaths::GetFontPath(filename);
}

void SORE_Font::Font::LoadFace(unsigned int height)
{
    ENGINE_LOG(SORE_Logging::LVL_DEBUG3,
               boost::format("Loading face for %d px %s") %
               height % GetFilename());

    FT_Set_Pixel_Sizes( face, 0, height);
    characters[height] = new CharInfo[NUM_CHARACTERS];

    unsigned int glyphWidth = 0;
    unsigned int glyphHeight = 0;

    CharInfoInternal* buffers = new CharInfoInternal[NUM_CHARACTERS];

    for(unsigned char i=0; i<NUM_CHARACTERS; ++i)
    {
        unsigned int w, h;
        LoadCharacter(i, height, buffers[i], w, h);
        if(w > glyphWidth)
            glyphWidth = w;
        if(h > glyphHeight)
            glyphHeight = h;
    }

    const unsigned int GLYPH_COLS = 16;
    const unsigned int GLYPH_ROWS = 8;

    assert(GLYPH_ROWS * GLYPH_COLS == NUM_CHARACTERS);

    unsigned int texWidth = next_p2(GLYPH_COLS*glyphWidth);
    unsigned int texHeight = next_p2(GLYPH_ROWS*glyphHeight);
    glyphWidth = texWidth / GLYPH_COLS;
    glyphHeight = texHeight / GLYPH_ROWS;

    GLint texSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
    unsigned int maxTex = texSize;
    if(texWidth > maxTex || texHeight > maxTex)
    {
        ENGINE_LOG(SORE_Logging::LVL_ERROR,
                   boost::format("Attempting to create font texture of size "
                                 "%d x %d, which is larger than the maximum "
                                 "(%d)") % texWidth % texHeight % texSize);
        for(unsigned int i = 0; i < NUM_CHARACTERS; ++i)
            delete[] buffers[i].data;
        delete[] buffers;
        return;
    }

    GLubyte* texture = new GLubyte[texWidth * texHeight * 4];
    memset(texture, 0, texWidth * texHeight * 4);

    for(unsigned int i = 0; i < NUM_CHARACTERS; ++i)
    {
        unsigned int x = i % GLYPH_COLS;
        unsigned int y = i / GLYPH_COLS;
        x *= glyphWidth;
        y *= glyphHeight;

        for(unsigned int j = 0; j < buffers[i].height; ++j)
        {
            GLubyte* destIndex = texture + 4*((y + j) * texWidth + x);
            GLubyte* srcIndex = buffers[i].data + 4*j*buffers[i].width;
            assert(4*((y + j) * texWidth + x) + 4*buffers[i].width <
                   4 * texWidth * texHeight);
            memcpy(destIndex, srcIndex, 4*buffers[i].width);
        }
    }

    textures[height] = SORE_Graphics::Texture2DPtr(
        new SORE_Graphics::Texture2D(
            texture, GL_RGBA, GL_RGBA, texWidth, texHeight));

    for(unsigned int i = 0; i < NUM_CHARACTERS; ++i)
    {
        if(buffers[i].width && buffers[i].height)
        {
            float xBlock = static_cast<float>(glyphWidth) / texWidth;
            float yBlock = static_cast<float>(glyphHeight) / texHeight;
            unsigned int x = i % GLYPH_COLS;
            unsigned int y = i / GLYPH_COLS;

            float xMin = x * xBlock;
            float yMin = y * yBlock;
            float xMax = xMin +
                static_cast<float>(buffers[i].width) / glyphWidth * xBlock;
            float yMax = yMin +
                static_cast<float>(buffers[i].height) / glyphHeight * yBlock;

            SORE_Math::Rect<float> bounds(
                0.0f,
                static_cast<float>(buffers[i].width),
                0.0f,
                static_cast<float>(buffers[i].height));
            SORE_Math::Rect<float> texCoords(xMin, xMax, yMin, yMax);
            SORE_Graphics::GLSLShaderPtr shader =
                pool->GetResource<SORE_Graphics::GLSLShader>
                ("data/Shaders/default.shad");

            characters[height][i].renderable = SORE_Graphics::MakeSprite(
                bounds, texCoords, 0.0f, textures[height], shader,
                SORE_Graphics::LAYER3, SORE_Graphics::BLEND_SUBTRACTIVE);
            characters[height][i].renderable.SetTransform(buffers[i].transform);
        }
    }

    for(unsigned int i = 0; i < NUM_CHARACTERS; ++i)
        delete[] buffers[i].data;
    delete[] buffers;
    delete[] texture;
}

void SORE_Font::Font::LoadCharacter(char ch, unsigned int h,
                                    CharInfoInternal& info,
                                    unsigned int& width,
                                    unsigned int& height)
{
    FT_Render_Mode mode = FT_RENDER_MODE_NORMAL;
    if(h <= 24)
        mode = FT_RENDER_MODE_MONO; //NORMAL gives odd glitches with small fonts

    int err;
    if((err = FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), mode ) ))
    {
        ENGINE_LOG(SORE_Logging::LVL_ERROR,
                   boost::format("Error loading '%c' (error %d)")
                   % ch % err);
    }

    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    FT_Bitmap& bitmap=face->glyph->bitmap;

    width = info.width = bitmap.width;
    height = info.height = bitmap.rows;

    info.data = new GLubyte[ 4 * width * height];

    for(unsigned int j=0; j<height;j++)
    {
        for(unsigned int i=0; i<width; i++)
        {
            //(r, g, b, a) = (1.0f, 1.0f, 1.0f, v)
            GLubyte value = (i>=width || j>=height) ?
                0 : bitmap.buffer[i + width*j];
            info.data[4*(i+j*width)+0] = 255;
            info.data[4*(i+j*width)+1] = 255;
            info.data[4*(i+j*width)+2] = 255;
            info.data[4*(i+j*width)+3] = value;

        }
    }

    unsigned int index = static_cast<unsigned int>(ch);
    if(width && height)
    {
        info.transform = boost::shared_ptr<SORE_Math::Matrix4<float> >(
            new SORE_Math::Matrix4<float>(
                SORE_Math::Matrix4<float>::GetTranslation(
                    static_cast<float>(face->glyph->bitmap_left),
                    static_cast<float>(bitmap.rows-face->glyph->bitmap_top) +
                    (h - bitmap.rows),
                    0.0f)));
    }
    characters[h][index].advance = static_cast<float>(
        face->glyph->advance.x >> 6);
}

const SORE_Font::CharInfo& SORE_Font::Font::GetCharacter(unsigned int height, char c)
{
    if(c > 127 && c < 0)
    {
        ENGINE_LOG(SORE_Logging::LVL_ERROR,
                   "Attempted to get non-existent character");
        return characters[height][0];
    }
    else
    {
        if(characters.find(height)==characters.end())
            LoadFace(height);
        return characters[height][static_cast<unsigned int>(c)];
    }
}

std::string SORE_Font::FontPaths::GetFontPath(const std::string& name)
{
    if(fontPaths.size()==0) InitPaths();

    std::string full = name;
    bool good = false;

    std::vector<std::string>::iterator it = fontPaths.begin();
    while(it!=fontPaths.end())
    {
        SORE_FileIO::InFile fontObj(full.c_str());
        if(fontObj.strm().good())
        {
            good = true;
            break;
        }
        full = *it;
        full += name;
        ++it;
    }
    if(good)
    {
        return full;
    }
    else
    {
        return name;
    }
}
