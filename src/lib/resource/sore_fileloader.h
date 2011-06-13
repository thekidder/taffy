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

#ifndef SORE_FILELOADER_H
#define SORE_FILELOADER_H

#include <sore_fileio.h>

#include <string>

namespace SORE_Resource
{
    // Basic loader infrastructure for many filetypes - provides loading 
    // resources from a packagecache, with a base filepath. Should be 
    // overloaded for specific resource types
    template<typename T>
    class FileResourceLoader
    {
    public:
        FileResourceLoader(
            SORE_FileIO::PackageCache& packageCache_, 
            const std::string& basePath_,
            const std::string& proxyName_);

        virtual T* Load(const std::string& path) = 0;
        T* LoadProxy();
    protected:
        SORE_FileIO::InFile* LoadFile(const std::string& path);
        SORE_FileIO::InFile* LoadFileWithoutBasePath(const std::string& path);
    private:
        SORE_FileIO::PackageCache& packageCache;
        std::string basePath;
        std::string proxyName;
    };

    template<typename T>
    FileResourceLoader<T>::FileResourceLoader(
        SORE_FileIO::PackageCache& packageCache_, 
        const std::string& basePath_,
        const std::string& proxyName_)
        : packageCache(packageCache_), basePath(basePath_), proxyName(proxyName_)
    {
    }

    template<typename T>
    T* FileResourceLoader<T>::LoadProxy()
    {
        if(proxyName.empty())
            return 0;
        else
            return Load(proxyName);
    }

    template<typename T>
    SORE_FileIO::InFile* FileResourceLoader<T>::LoadFile(const std::string& path)
    {
        return new SORE_FileIO::InFile((basePath + path).c_str(), &packageCache);
    }

    template<typename T>
    SORE_FileIO::InFile* FileResourceLoader<T>::LoadFileWithoutBasePath(const std::string& path)
    {
        return new SORE_FileIO::InFile(path.c_str(), &packageCache);
    }
}

#endif