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

#include <fstream>

#include "sore_texture.h"
#include "sore_util.h"
#include "sore_fileio.h"
#include "sore_allgl.h"
#include "sore_logger.h"

namespace SORE_Graphics
{

    void Texture2D::Load()
    {
        if(handle!=0)
            Unload();
        char ext[10];
        SORE_Utility::GetFileExt(GetFilename().c_str(), ext);
        if(strcmp(ext, "tga")==0)
        {
            LoadTGA(GetFilename().c_str());
        }
        else
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       boost::format("Unknown file format %s") % ext);
        }
    }

    void Texture2D::LoadTGA(const char* filename)
    {
        handle = 0;
        bool failed = false;
        GLubyte header[18];

        SORE_FileIO::InFile* file = File(filename);
        if(!file->strm().good() )
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       boost::format("Could not open texture file '%s'") % filename);
            return;
        }
        file->strm().read(reinterpret_cast<char*>(header), 18);
        if(file->strm().gcount() < 18)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       "Could not read header...corrupted file?");
            return;
        }

        ENGINE_LOG(SORE_Logging::LVL_DEBUG2, "Loaded header");
        ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Name: %s") % filename);
        ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Width: %d Height: %d")
                   % int(header[12]+header[13]*256) % int(header[14]+header[15]*256));
        ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("BPP: %d Image type: %d")
                   % (int)header[16] % (int)header[2]);

        //do some basic checks to make sure we can handle the file

        GLubyte requested[18] = {0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        GLubyte mask     [18] = {0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0};

        for(int i=0;i<18;i++)
        {
            if(mask[i]!=0 && requested[i]!=header[i])
            {
                failed = true;
            }
        }

        if(failed)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not read file: Unsupported");
            return;
        }

        int width, height, bpp;

        bpp    = int(header[16]);
        width  = int(header[12])+((int)header[13])*256;
        height = int(header[14])+((int)header[15])*256;

        char* filler = new char[int(header[0])];

        file->strm().read(filler, static_cast<int>(header[0]));
        if(file->strm().gcount()!=static_cast<int>(header[0]))
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       "Could not read filler...corrupted file?");
            return;
        }

        delete[] filler;

        unsigned int dataSize = width*height*(bpp/8);

        GLubyte* imgData = new GLubyte[dataSize];
        GLubyte temp;

        file->strm().read(reinterpret_cast<char*>(imgData), dataSize);
        if(file->strm().gcount()!=dataSize)
        {
            delete[] imgData;
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       "Could not read image data...corrupted file?");
            return;
        }
        delete file;

        for(size_t i=0;i<dataSize;i+=(bpp/8))
        {
            temp = imgData[i];
            imgData[i] = imgData[i+2];
            imgData[i+2] = temp;
        }

        GLint type;

        switch(bpp)
        {
        case 24:
            type = GL_RGB;
            break;
        case 32:
            type = GL_RGBA;
            break;
        default:
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       "Unrecognized image type - only supports 24 or 32 bit "
                       "(uncompressed) TGAs");
            delete[] imgData;
            return;
        }

        LoadFromData(imgData, type, type, width, height);

        delete[] imgData;
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

    void Texture2D::LoadFromData(const unsigned char* data, GLint internalFormat,
                                 GLenum format, unsigned int width, unsigned height)
    {
        w = width;
        h = height;
        glGenTextures(1, &handle);
        glBindTexture(GL_TEXTURE_2D, handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width,
                      height, 0, format,
                      GL_UNSIGNED_BYTE, data);
    }

    Texture2D::Texture2D(SORE_Resource::WatchedFileArrayPtr wfa)
        : Resource(wfa), handle(0)
    {
        Load();
    }

    Texture2D::Texture2D(const unsigned char* data, GLint internalFormat,
                         GLenum format, unsigned int width, unsigned int height)
        : Resource(SORE_Resource::WatchedFileArrayPtr()), handle(0)
    {
        LoadFromData(data, internalFormat, format, width, height);

    }

    Texture2D::~Texture2D()
    {
        Unload();
    }

    bool Texture2D::operator<(const Texture2D& o) const
    {
        return handle < o.handle;
    }

    bool Texture2D::operator==(const Texture2D& o) const
    {
        return handle == o.handle;
    }

    std::string Texture2D::ProcessFilename(const std::string& file)
    {
        return file;
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

    unsigned int Texture2D::GetHandle() const
    {
        return handle;
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
