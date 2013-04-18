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

#include <sore_logger.h>
#include <sore_texture2d_loader.h>

SORE_Resource::Texture2DLoader::Texture2DLoader(
    SORE_FileIO::PackageCache& packageCache_, 
    const std::string& basePath_,
    const std::string& proxyName_)
    : FileResourceLoader<Texture2D>(packageCache_, basePath_, proxyName_)
{
}

SORE_Resource::Texture2D* SORE_Resource::Texture2DLoader::Load(const std::string& path)
{
    SORE_FileIO::InFile* file = LoadFile(path);

    bool failed = false;
    GLubyte header[18];

    if(!file->strm().good() )
    {
        delete file;
        ENGINE_LOG(SORE_Logging::LVL_ERROR,
                    boost::format("Could not open texture file '%s'") % path);
        return 0;
    }
    file->strm().read(reinterpret_cast<char*>(header), 18);
    if(file->strm().gcount() < 18)
    {
        delete file;
        ENGINE_LOG(SORE_Logging::LVL_ERROR,
                    "Could not read header...corrupted file?");
        return 0;
    }

    ENGINE_LOG(SORE_Logging::LVL_DEBUG2, "Loaded header");
    ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Name: %s") % path);
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
        delete file;
        ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not read file: Unsupported");
        return 0;
    }

    int width, height, bpp;

    bpp    = int(header[16]);
    width  = int(header[12])+((int)header[13])*256;
    height = int(header[14])+((int)header[15])*256;

    char* filler = new char[int(header[0])];
    file->strm().read(filler, static_cast<int>(header[0]));
    delete[] filler;

    if(file->strm().gcount()!=static_cast<int>(header[0]))
    {
        delete file;
        ENGINE_LOG(SORE_Logging::LVL_ERROR,
                    "Could not read filler...corrupted file?");
        return 0;
    }

    unsigned int dataSize = width*height*(bpp/8);

    GLubyte* imgData = new GLubyte[dataSize];
    GLubyte temp;

    file->strm().read(reinterpret_cast<char*>(imgData), dataSize);
    std::streamsize readSoFar = file->strm().gcount();
    delete file;
    if(readSoFar != dataSize)
    {
        delete[] imgData;
        ENGINE_LOG(SORE_Logging::LVL_ERROR,
                    "Could not read image data...corrupted file?");
        return 0;
    }


    //flip bytes to read in from BGR -> RGB
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
        return 0;
    }

    Texture2D* t = new Texture2D(imgData, type, type, width, height);

    delete imgData;

    return t;
}