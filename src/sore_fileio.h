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

#ifndef  SORE_FILEIO_H
#define  SORE_FILEIO_H

#include <cstdio>
#include <iosfwd>
#include <limits>

#include <boost/function.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/shared_ptr.hpp>

#include <zlib.h>

#include "sore_gamekernel.h"

namespace SORE_FileIO
{
    class SORE_EXPORT GenericPkgFileBuf
    {
    public:
        GenericPkgFileBuf(std::ifstream& package_, unsigned int pos_, unsigned int size_);
        virtual ~GenericPkgFileBuf() {}

        virtual std::streamsize read(char* s, std::streamsize n) = 0;
        size_t size() const;
    protected:
        std::ifstream& package;
        unsigned int pos, fileSize;
        unsigned int currentPos;
    };

    class SORE_EXPORT UncompressedPkgFileBuf : public GenericPkgFileBuf
    {
    public:
        UncompressedPkgFileBuf(std::ifstream& package_, unsigned int pos_, unsigned int size_);
        ~UncompressedPkgFileBuf();

        std::streamsize read(char* s, std::streamsize n);
    };

    class SORE_EXPORT CompressedPkgFileBuf : public GenericPkgFileBuf
    {
    public:
        CompressedPkgFileBuf(std::ifstream& package_, unsigned int pos_, unsigned int size_,
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

    class SORE_EXPORT PkgFileBuf
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

    class SORE_EXPORT PackageCache
    {
    public:
        PackageCache();
        ~PackageCache();

        void AddPackage(const char* packagename);

        bool Contains(const char* filename) const;
        PkgFileBuf* GetFileBuf(const char* filename);
    private:
        struct file_info
        {
            unsigned short int fileID;
            unsigned short int parentID;
            bool               directory;
            bool               compressed;

            std::string        package;
            std::string        filename;

            size_t       pos;
            size_t       size;
            size_t       sizeRaw;
        };
        typedef std::map<std::string, file_info> cache_type;

        void CloseAllPackages();
        std::string BuildFullName(file_info& file);
        cache_type::iterator FileInfo(unsigned int id);
        //opens package if not open
        std::ifstream& GetPackage(const char* packagename);

        //all cached files and where they are
        //stored as a (fullname, file_info) pair
        cache_type cache;
        //which packages have currently been cached
        std::vector<std::string> cachedPackages;
        //open file handles
        std::map<std::string, std::ifstream*> openPackages;
    };

    class SORE_EXPORT InFile
    {
    public:
        InFile(const char* filename, PackageCache* cache = NULL);
        ~InFile();

        std::istream& strm();
        size_t size() const;
    private:
        std::istream* in;
        PkgFileBuf* buf;
    };

    typedef boost::function<void (std::string)> file_callback;

    //only implemented on local files now (no packages)
    void SORE_EXPORT Notify(std::string filename, file_callback callback);
    bool SORE_EXPORT InitFileNotify(SORE_Kernel::GameKernel* gk);
}

#endif
