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
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
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

// suppress warnings in visual studio with std::copy
#define _SCL_SECURE_NO_WARNINGS

#include <sore_fileio.h>
#include <sore_font.h>
#include <sore_geometry.h>
#include <sore_logger.h>
#include <sore_glslshader.h>
#include <sore_sprite.h>
#include <sore_texture2d.h>

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <string.h>
#include <vector>

#include <boost/lexical_cast.hpp>

//number of characters to load for each face
//we'll load the full ASCII set
const static unsigned int CHARACTER_OFFSET = 32;
const static unsigned int NUM_CHARACTERS   = 95;

std::vector<std::string> SORE_Resource::FontPaths::fontPaths;
int SORE_Resource::Font::numFonts = 0;
FT_Library SORE_Resource::Font::library;

inline static int next_p2 (int a )
{
    int rval=1;
    while(rval<a) rval<<=1;
    return rval;
}

SORE_Resource::Font::Font(FT_Byte* faceData_, size_t length)
{
    if(numFonts == 0)
    {
        // TODO: sore exception handling
        if (FT_Init_FreeType( &library ))
            throw std::runtime_error("Could not initialize Freetype");
    }

    faceData.resize(length);
    std::copy(faceData_, faceData_ + length, &faceData[0]);

    FT_Error error;
    error = FT_New_Memory_Face(
        library, &faceData[0], static_cast<FT_Long>(length),
        0, &face);

    if (error)
    {
        throw std::runtime_error(
            (boost::format("Font load failed: Freetype error code %d") % error).str());
    }
    ++numFonts;
}

void DeleteCharacters(std::pair<unsigned int, SORE_Resource::CharInfo*> chars)
{
    delete[] chars.second;
}

SORE_Resource::Font::~Font()
{
    --numFonts;
    for_each(characters.begin(), characters.end(), &DeleteCharacters);
    FT_Done_Face(face);
    if(numFonts == 0)
    {
        ENGINE_LOG(SORE_Logging::LVL_INFO, "Destroyed freetype library");
        FT_Done_FreeType(library);
    }
}
void SORE_Resource::Font::LoadFace(unsigned int height)
{
    FT_Set_Pixel_Sizes( face, 0, height);
    characters[height] = new CharInfo[NUM_CHARACTERS];

    unsigned int glyphWidth = 0;
    unsigned int glyphHeight = 0;

    CharInfoInternal* buffers = new CharInfoInternal[NUM_CHARACTERS];

    for(unsigned char i=0; i<NUM_CHARACTERS; ++i)
    {
        unsigned int w, h;
        LoadCharacter(i + CHARACTER_OFFSET, height, buffers[i], w, h);
        if(w > glyphWidth)
            glyphWidth = w;
        if(h > glyphHeight)
            glyphHeight = h;
    }

    const unsigned int GLYPH_COLS = 12;
    const unsigned int GLYPH_ROWS = 8;

    assert(GLYPH_ROWS * GLYPH_COLS >= NUM_CHARACTERS);

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

    if(textures.find(height) == textures.end())
    {
        textures[height] = new SORE_Resource::Texture2D(
            texture, GL_RGBA, GL_RGBA, texWidth, texHeight);
    }
    else
    {
        textures[height]->LoadFromData(texture, GL_RGBA, GL_RGBA, texWidth, texHeight);
    }

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

            characters[height][i].texture = textures[height];

            characters[height][i].vertices[0].x     = buffers[i].x;
            characters[height][i].vertices[0].y     = buffers[i].y;
            characters[height][i].vertices[0].z     = 0.0f;
            characters[height][i].vertices[0].tex0i = xMin;
            characters[height][i].vertices[0].tex0j = yMin;

            characters[height][i].vertices[1].x     = buffers[i].x;
            characters[height][i].vertices[1].y     = buffers[i].y + buffers[i].height;
            characters[height][i].vertices[1].z     = 0.0f;
            characters[height][i].vertices[1].tex0i = xMin;
            characters[height][i].vertices[1].tex0j = yMax;

            characters[height][i].vertices[2].x     = buffers[i].x + buffers[i].width;
            characters[height][i].vertices[2].y     = buffers[i].y;
            characters[height][i].vertices[2].z     = 0.0f;
            characters[height][i].vertices[2].tex0i = xMax;
            characters[height][i].vertices[2].tex0j = yMin;

            characters[height][i].vertices[3].x     = buffers[i].x + buffers[i].width;
            characters[height][i].vertices[3].y     = buffers[i].y + buffers[i].height;
            characters[height][i].vertices[3].z     = 0.0f;
            characters[height][i].vertices[3].tex0i = xMax;
            characters[height][i].vertices[3].tex0j = yMax;
        }
    }

    for(unsigned int i = 0; i < NUM_CHARACTERS; ++i)
        delete[] buffers[i].data;
    delete[] buffers;
    delete[] texture;
}

void SORE_Resource::Font::LoadCharacter(char ch, unsigned int h,
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

    unsigned int index = static_cast<unsigned int>(ch - CHARACTER_OFFSET);
    if(width && height)
    {
        info.x = static_cast<float>(face->glyph->bitmap_left);
        info.y = static_cast<float>(
            bitmap.rows-face->glyph->bitmap_top + (h - bitmap.rows));
    }
    characters[h][index].advance = static_cast<float>(
        face->glyph->advance.x >> 6);
}

const SORE_Resource::CharInfo& SORE_Resource::Font::GetCharacter(unsigned int height, char c)
{
    if(c >= CHARACTER_OFFSET + NUM_CHARACTERS && c < CHARACTER_OFFSET)
    {
        ENGINE_LOG(SORE_Logging::LVL_ERROR,
                   "Attempted to get non-existent character");
        return characters[height][0];
    }
    else
    {
        if(characters.find(height)==characters.end())
        {
            ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Loading face for height %d: %c") % height % c);
            LoadFace(height);
        }
        return characters[height][static_cast<unsigned int>(c - CHARACTER_OFFSET)];
    }
}

float SORE_Resource::Font::Width(unsigned int height, const std::string str)
{
    if(characters.find(height)==characters.end())
    {
        ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Loading face for height %d") % height);
        LoadFace(height);
    }

    float width = 0.0f;
    for(std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        const SORE_Resource::CharInfo& c = GetCharacter(height, *it);
        width += c.advance;
    }
    return width;
}

std::string SORE_Resource::FontPaths::GetFontPath(const std::string& name)
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
