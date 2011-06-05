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

// disable warning about std::copy being called unsafely in CompressedPkgFileBuf::read
#pragma warning(push)
#pragma warning(disable: 4996)

#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#pragma warning(pop)

#include <zlib.h>

#include <sore_fileio.h>
#include <sore_logger.h>

#define MODULE_FILEIO 1

namespace SORE_FileIO
{
    const unsigned int MAX_SDP_MAJOR = 0;
    const unsigned int MAX_SDP_MINOR = 2;

    const unsigned int CHUNK = 4096;

    //implementation class declarations

    class GenericPkgFileBuf
    {
    public:
        GenericPkgFileBuf(
            std::ifstream& package_, unsigned int pos_, unsigned int size_);
        virtual ~GenericPkgFileBuf() {}

        virtual std::streamsize read(char* s, std::streamsize n) = 0;
        size_t size() const;
    protected:
        std::ifstream& package;
        unsigned int pos, fileSize;
        unsigned int currentPos;
    };

    class UncompressedPkgFileBuf : public GenericPkgFileBuf
    {
    public:
        UncompressedPkgFileBuf(
            std::ifstream& package_, unsigned int pos_, unsigned int size_);
        ~UncompressedPkgFileBuf();

        std::streamsize read(char* s, std::streamsize n);
    };

    class CompressedPkgFileBuf : public GenericPkgFileBuf
    {
    public:
        CompressedPkgFileBuf(
            std::ifstream& package_, unsigned int pos_, unsigned int size_,
            unsigned int sizeRaw_);
        ~CompressedPkgFileBuf();

        std::streamsize read(char* s, std::streamsize n);
    private:
        unsigned int sizeRaw;

        z_stream strm;

        unsigned int num_out;
        std::vector<unsigned char> out;

        bool eof;
    };

    class PkgFileBuf
    {
    public:
        typedef char        char_type;
        typedef boost::iostreams::source_tag  category;

        PkgFileBuf(std::ifstream& package, unsigned int pos, unsigned int size,
                   unsigned int sizeRaw);
        ~PkgFileBuf();

        std::streamsize read(char* s, std::streamsize n);
        size_t size() const;
    private:
        boost::shared_ptr<GenericPkgFileBuf> d_ptr;

        unsigned int raw;
    };


    PackageCache::PackageCache()
    {
    }

    PackageCache::~PackageCache()
    {
        CloseAllPackages();
    }

    void PackageCache::CloseAllPackages()
    {
        std::map<std::string, std::ifstream*>::iterator it;
        for(it = openPackages.begin(); it != openPackages.end(); ++it)
        {
            it->second->close();
            delete it->second;
        }
    }

    std::ifstream& PackageCache::GetPackage(const char* packagename)
    {
        if(openPackages.find(packagename) == openPackages.end())
        {
            std::ifstream* packageFile = new std::ifstream(
                packagename, std::ios::binary);
            openPackages.insert(std::make_pair(packagename, packageFile));
            if(!packageFile->good())
                ENGINE_LOG(SORE_Logging::LVL_ERROR, "could not open file");
        }
        return *(openPackages[packagename]);
    }

    PackageCache::cache_type::iterator PackageCache::FileInfo(unsigned int id)
    {
        std::map<std::string, file_info>::iterator it;
        for(it = cache.begin(); it != cache.end(); ++it)
        {
            if(it->second.fileID == id)
                return it;
        }
        return cache.end();
    }

    PackageCache::cache_type::iterator PackageCache::FileInfo(const char* filename)
    {
        std::map<std::string, file_info>::iterator it;
        for(it = cache.begin(); it != cache.end(); ++it)
        {
            if(it->first == filename)
                return it;
        }
        return cache.end();
    }


    std::string PackageCache::BuildFullName(file_info& file)
    {
        std::string full = file.filename;
        file_info parent = file;
        while(parent.parentID != 0)
        {
            parent = FileInfo(parent.parentID)->second;
            full = parent.filename + "/" + full;
        }
        return full;
    }

    std::string PackageCache::PackageFilename(const char* filename)
    {
        cache_type::iterator it;
        if((it = FileInfo(filename)) == cache.end())
            return "";
        return it->second.package;
    }

    void PackageCache::AddPackage(const char* packagename)
    {
        char header[7];

        if(find(cachedPackages.begin(), cachedPackages.end(), packagename) !=
           cachedPackages.end())
            return; //package is already in cache
        cachedPackages.push_back(packagename);

        std::ifstream package(packagename, std::ios::binary);
        package.read(header, 7);

        if(header[0]!='S' || header[1]!='D' || header[2]!='P')
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       boost::format("Failed to open %s: not a valid SDP package")
                       % packagename);
            return;
        }

        unsigned int version_major = static_cast<unsigned int>(header[3]);
        unsigned int version_minor = static_cast<unsigned int>(header[4]);

        if(version_major > MAX_SDP_MAJOR ||
           (version_major == MAX_SDP_MAJOR && version_minor > MAX_SDP_MINOR))
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       boost::format("Failed to open %s: SDP version is too "
                                     "high (%d.%d)")
                       % version_major % version_minor);
            return;
        }

        unsigned short numFiles = static_cast<unsigned short>(
            header[5]+(header[6]<<8));
        ENGINE_LOG(SORE_Logging::LVL_INFO,
                   boost::format("Caching %s with %d files and directories")
                   % packagename % numFiles);

        //we need to reassign IDs given how many files are already cached so we
        //don't mangle existing ids
        unsigned int idPrefix = cache.size();

        //we also need to reassign folder IDs if a folder already exists in the cache
        std::map<unsigned int, unsigned int> reassignedIDs;

        char flags;
        int pos;
        for(unsigned int i = 0; i < numFiles; ++i)
        {
            pos = 0;

            file_info temp;
            unsigned short tempShort;
            package.read(reinterpret_cast<char*>(&tempShort), 2);
            temp.fileID = tempShort;
            package.read(&flags, 1);
            package.read(reinterpret_cast<char*>(&tempShort), 2);
            temp.parentID = tempShort;

            temp.fileID += idPrefix;
            if(temp.parentID == 65535)
                temp.parentID = 0;
            else
                temp.parentID += idPrefix;

            char c;
            temp.filename = "";
            while(1)
            {
                package.read(&c, 1);
                if(c == '\0') break;
                temp.filename += c;
            }

            if(reassignedIDs.find(temp.parentID)!=reassignedIDs.end())
            {
                temp.parentID = reassignedIDs[temp.parentID];
            }

            if(!(flags & 0x01))
            {
                temp.directory = false;
                package.read(reinterpret_cast<char*>(&temp.pos), 4);
                package.read(reinterpret_cast<char*>(&temp.size), 4);
                //file is compressed?
                if(flags & 0x02)
                    package.read(reinterpret_cast<char*>(&temp.sizeRaw), 4);
            }
            else
            {
                temp.directory = true;
                std::string fullname = BuildFullName(temp);
                std::map<std::string, file_info>::iterator it;
                //if directory already exists in cache, reuse it
                if((it = cache.find(fullname)) != cache.end())
                {
                    reassignedIDs.insert(std::make_pair(temp.fileID, it->second.fileID));
                    continue;
                }
            }
            if(flags & 0x02)
                temp.compressed = true;
            else
                temp.compressed = false;
            temp.package = packagename;
            std::string fullname = BuildFullName(temp);
            cache.insert(std::make_pair(fullname, temp));
        }
        package.close();
    }

    bool PackageCache::Contains(const char* filename) const
    {
        if(cache.find(filename) != cache.end())
            return true;
        return false;
    }

    PkgFileBuf* PackageCache::GetFileBuf(const char* filename)
    {
        file_info& fi = cache[filename];
        std::ifstream& strm = GetPackage(fi.package.c_str());
        if(!fi.compressed)
            return new PkgFileBuf(strm, fi.pos, fi.size, 0);
        else
            return new PkgFileBuf(strm, fi.pos, fi.size, fi.sizeRaw);
    }

    GenericPkgFileBuf::GenericPkgFileBuf(std::ifstream& package_, unsigned int pos_,
                                         unsigned int size_) :
        package(package_), pos(pos_), fileSize(size_), currentPos(pos)
    {
    }

    size_t GenericPkgFileBuf::size() const
    {
        return fileSize;
    }

    UncompressedPkgFileBuf::UncompressedPkgFileBuf(
        std::ifstream& package_, unsigned int pos_, unsigned int size_) :
        GenericPkgFileBuf(package_, pos_, size_)
    {
    }

    UncompressedPkgFileBuf::~UncompressedPkgFileBuf()
    {
    }

    std::streamsize UncompressedPkgFileBuf::read(char* s, std::streamsize n)
    {
        if(currentPos - pos == fileSize) return -1;
        unsigned int bytesToRead = std::min(fileSize - (currentPos - pos),
                                            static_cast<unsigned int>(n));
        package.seekg(currentPos);
        package.read(s, bytesToRead);
        return package.gcount();
    }

    CompressedPkgFileBuf::CompressedPkgFileBuf(
        std::ifstream& package_, unsigned int pos_, unsigned int size_,
        unsigned int sizeRaw_)
        : GenericPkgFileBuf(package_, pos_, size_), sizeRaw(sizeRaw_),
          num_out(0), eof(false)
    {
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;
        int ret = inflateInit(&strm);
        if(ret != Z_OK)
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Failed to initialize zlib stream");
    }

    CompressedPkgFileBuf::~CompressedPkgFileBuf()
    {
        inflateEnd(&strm);
    }

    std::streamsize CompressedPkgFileBuf::read(char* s, std::streamsize n)
    {
        if(eof) return -1;
        int ret = Z_OK;
        package.seekg(currentPos);
        while(num_out < n && ret != Z_STREAM_END)
        {
            char in[CHUNK];
            unsigned int toRead = (sizeRaw+pos)-currentPos;
            toRead = std::min(toRead, CHUNK);
            package.read(in, toRead);
            unsigned int num_in = static_cast<unsigned int>(package.gcount());
            currentPos += num_in;

            strm.avail_in = static_cast<uInt>(num_in);
            strm.next_in = reinterpret_cast<Bytef*>(in);

            do
            {
                out.resize(num_out + CHUNK);
                strm.avail_out = CHUNK;
                strm.next_out = &(out[num_out]);

                ret = inflate(&strm, Z_NO_FLUSH);
                assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
                switch (ret)
                {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;     /* and fall through */
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    inflateEnd(&strm);
                    ENGINE_LOG(SORE_Logging::LVL_ERROR, "Internal zlib error on read");
                }
                num_out += (CHUNK - strm.avail_out);

            }
            while(strm.avail_out == 0);
        }
        unsigned int have = std::min(static_cast<unsigned int>(n), num_out);
        if(have < n)
            eof = true;

        std::copy(out.begin(), out.begin() + have, s);
        out.erase(out.begin(), out.begin() + have);
        num_out -= have;
        return have;
    }

    PkgFileBuf::PkgFileBuf(std::ifstream& package, unsigned int pos, unsigned int size,
                           unsigned int sizeRaw) : raw(sizeRaw)
    {
        if(sizeRaw == 0)
        {
            boost::shared_ptr<GenericPkgFileBuf> p(
                new UncompressedPkgFileBuf(package, pos, size));
            d_ptr = p;
        }
        else
        {
            boost::shared_ptr<GenericPkgFileBuf> p(
                new CompressedPkgFileBuf(package, pos, size, sizeRaw));
            d_ptr =p;
        }
    }

    size_t PkgFileBuf::size() const
    {
        return d_ptr->size();
    }

    std::streamsize PkgFileBuf::read(char* s, std::streamsize n)
    {
        return d_ptr->read(s, n);
    }

    PkgFileBuf::~PkgFileBuf()
    {
    }

    InFile::InFile(const char* filename, PackageCache* cache) : in(0), buf(0)
    {
        if(cache)
        {
            if(cache->Contains(filename))
            {
                buf = cache->GetFileBuf(filename);
                in = new boost::iostreams::stream<PkgFileBuf>(*buf);
                return;
            }
        }
        std::ifstream* in_temp = new std::ifstream;
        in_temp->open(filename, std::ios::binary);
        in = in_temp;
    }

    InFile::~InFile()
    {
        delete in;
        delete buf;
    }

    std::istream& InFile::strm()
    {
        return *in;
    }

    size_t InFile::size() const
    {
        if(buf)
        {
            return buf->size();
        }
        else
        {
            std::streampos cur = in->tellg();
            in->seekg(0);
            std::streampos begin = in->tellg();
            in->seekg(0, std::ios::end);
            std::streampos end = in->tellg();
            size_t size = static_cast<size_t>(end - begin);
            in->seekg(cur);
            return size;
        }
    }
}

#undef MODULE_FILEIO
