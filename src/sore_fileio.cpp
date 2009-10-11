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

#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <zlib.h>

#include "sore_fileio.h"
#include "sore_logger.h"

#define MODULE_FILEIO 1

namespace SORE_FileIO
{
    const unsigned int MAX_SDP_MAJOR = 0;
    const unsigned int MAX_SDP_MINOR = 2;

    //const size_t CHUNK = 131072;
    const int RATIO = 4;

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
            std::ifstream* packageFile = new std::ifstream(packagename, std::ios::binary);
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

    void PackageCache::AddPackage(const char* packagename)
    {
        char header[7];

        if(find(cachedPackages.begin(), cachedPackages.end(), packagename) != cachedPackages.end())
            return; //package is already in cache
        cachedPackages.push_back(packagename);

        std::ifstream package(packagename, std::ios::binary);
        package.read(header, 7);

        if(header[0]!='S' || header[1]!='D' || header[2]!='P')
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       boost::format("Failed to open %s: not a valid SDP package") % packagename);
            return;
        }

        unsigned int version_major = static_cast<unsigned int>(header[3]);
        unsigned int version_minor = static_cast<unsigned int>(header[4]);

        if(version_major > MAX_SDP_MAJOR ||
           (version_major == MAX_SDP_MAJOR && version_minor > MAX_SDP_MINOR))
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       boost::format("Failed to open %s: SDP version is too high (%d.%d)")
                       % version_major % version_minor);
            return;
        }

        unsigned short numFiles = static_cast<unsigned short>(header[5] + (header[6]<<8));
        ENGINE_LOG(SORE_Logging::LVL_INFO,
                   boost::format("Caching %s with %d files and directories")
                   % packagename % numFiles);

        //we need to reassign IDs given how many files are already cached so we don't mangle
        //existing ids
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
                ENGINE_LOG_M(SORE_Logging::LVL_DEBUG2,
                             boost::format("Reassigning parent for %s to %d")
                             % temp.filename % reassignedIDs[temp.parentID], MODULE_FILEIO);
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
        package(package_), pos(pos_), size(size_), currentPos(pos)
    {
    }

    UncompressedPkgFileBuf::UncompressedPkgFileBuf(std::ifstream& package_, unsigned int pos_,
                           unsigned int size_) :
        GenericPkgFileBuf(package_, pos_, size_)
    {
    }

    UncompressedPkgFileBuf::~UncompressedPkgFileBuf()
    {
    }

    std::streamsize UncompressedPkgFileBuf::read(char* s, std::streamsize n)
    {
        if(currentPos - pos == size) return -1;
        unsigned int bytesToRead = std::min(size - (currentPos - pos),
                                            static_cast<unsigned int>(n));
        package.seekg(currentPos);
        package.read(s, bytesToRead);
        return package.gcount();
    }

    CompressedPkgFileBuf::CompressedPkgFileBuf(std::ifstream& package_, unsigned int pos_,
                                               unsigned int size_, unsigned int sizeRaw_) :
        GenericPkgFileBuf(package_, pos_, size_),
        sizeRaw(sizeRaw_), num_in(0), num_out(0), eof(false)
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
        while(num_out < n && ret != Z_STREAM_END)
        {
            if(!num_in)
            {
                package.seekg(currentPos);
                char* inptr = reinterpret_cast<char*>(in);
                unsigned int toRead = (size+pos)-currentPos;
                toRead = std::min(toRead, CHUNK);
                package.read(inptr, CHUNK);
                num_in = package.gcount();
                currentPos += num_in;
            }
            strm.avail_in = num_in;
            strm.next_in = in;

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
        std::copy(out.begin() + have, out.end(), out.begin());
        num_out -= have;
        return have;
    }

    PkgFileBuf::PkgFileBuf(std::ifstream& package, unsigned int pos, unsigned int size,
                           unsigned int sizeRaw) : raw(sizeRaw)
    {
        if(sizeRaw == 0)
        {
            boost::shared_ptr<GenericPkgFileBuf> p(new UncompressedPkgFileBuf(package, pos, size));
            d_ptr = p;
        }
        else
        {
            boost::shared_ptr<GenericPkgFileBuf> p
                (new CompressedPkgFileBuf(package, pos, size, sizeRaw));
            d_ptr =p;
        }
    }

    std::streamsize PkgFileBuf::read(char* s, std::streamsize n)
    {
        return d_ptr->read(s, n);
    }

    PkgFileBuf::~PkgFileBuf()
    {
    }

    InFile::InFile(const char* filename, PackageCache* cache) : in(0)
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

#undef MODULE_FILEIO
