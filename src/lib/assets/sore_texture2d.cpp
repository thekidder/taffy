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

#include <fstream>

#include <sore_texture2d.h>
#include <sore_util.h>
#include <sore_fileio.h>
#include <sore_allgl.h>
#include <sore_logger.h>

namespace SORE_Resource
{
    Texture2D::Texture2D(const void* data, GLint internalFormat,
                         GLenum format, unsigned int width, unsigned int height)
        : handle(0)
    {
        LoadFromData(data, internalFormat, format, width, height);
    }

    Texture2D::~Texture2D()
    {
        Unload();
    }

    void Texture2D::SaveTGA(const char* filename)
    {
         char header[18] = {0};
         char wLow = static_cast< char>(w);
         char wHigh = static_cast< char>(w >> 8);

        char hLow = static_cast< char>(h);
        char hHigh = static_cast< char>(h >> 8);

        unsigned char bpp = 32;

        header[2] = 2; //image type
        header[12] = wLow; //width
        header[13] = wHigh;
        header[14] = hLow; //height
        header[15] = hHigh;
        header[16] = bpp;
        header[17] = 0x28; //origin in upper left

        unsigned char* data = new unsigned char[w*h*4];

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, handle);

        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        //TGA image stored BGR, need to flip byte order
        for(unsigned int i=0;i<w*h*4;i+=4)
        {
            char temp = data[i];

            data[i] = data[i+2];
            data[i+2] = temp;
        }

        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        file.write(header, 18);
        file.write(reinterpret_cast<char*>(data), w*h*4);
        file.close();
    }

    void Texture2D::LoadFromData(const void* data, GLint internalFormat,
                                 GLenum format, unsigned int width, unsigned height)
    {
        if(handle!=0)
            Unload();

        w = width;
        h = height;
        glGenTextures(1, &handle);
        glBindTexture(GL_TEXTURE_2D, handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        GLenum type = GL_UNSIGNED_BYTE;
        //interpret data as unsigned bytes unless a float format is specified
        const int NUM_FLOAT_FORMATS = 12;
        GLint floatFormats[NUM_FLOAT_FORMATS] = {
            GL_RGBA32F_ARB,
            GL_RGB32F_ARB,
            GL_ALPHA32F_ARB,
            GL_INTENSITY32F_ARB,
            GL_LUMINANCE32F_ARB,
            GL_LUMINANCE_ALPHA32F_ARB,
            GL_RGBA16F_ARB,
            GL_RGB16F_ARB,
            GL_ALPHA16F_ARB,
            GL_INTENSITY16F_ARB,
            GL_LUMINANCE16F_ARB,
            GL_LUMINANCE_ALPHA16F_ARB
        };
        for(int i = 0; i < NUM_FLOAT_FORMATS; ++i)
        {
            if(internalFormat == floatFormats[i])
                type = GL_FLOAT;
        }
        glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width,
                      height, 0, format,
                      type, data);
    }

    bool Texture2D::operator<(const Texture2D& o) const
    {
        return handle < o.handle;
    }

    bool Texture2D::operator==(const Texture2D& o) const
    {
        return handle == o.handle;
    }

    void Texture2D::Bind(GLSLShaderPtr shader,
                         const std::string& sampleName,
                         unsigned int textureSlot) const
    {
        assert(textureSlot < 8);
        shader->SetUniform1i(sampleName, textureSlot);
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, handle);
    }

    void Texture2D::Unload()
    {
        glDeleteTextures(1, &handle);
        handle = 0;
    }

    bool operator!=(const Texture2D& one, const Texture2D& two)
    {
        return !(one == two);
    }
}
