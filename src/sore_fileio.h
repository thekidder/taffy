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
#include "sore_task.h"

namespace SORE_FileIO
{
    class PkgFileBuf;

    class SORE_EXPORT PackageCache
    {
    public:
        PackageCache();
        ~PackageCache();

        void AddPackage(const char* packagename);

        bool Contains(const char* filename) const;
        PkgFileBuf* GetFileBuf(const char* filename);
        //return filename of the package containing filename, or empty string if
        //the file does not exist in this cache
        std::string PackageFilename(const char* filename);
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
        cache_type::iterator FileInfo(const char* filename);
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
        //a stream is non-copyable
        /*InFile(const InFile& o);
          InFile& operator=(const InFile& o);*/

        std::istream* in;
        PkgFileBuf* buf;
    };

    typedef boost::function<void (const std::string&)> file_callback;

    class FilesystemNotifier : public SORE_Kernel::Task
    {
    public:
        virtual ~FilesystemNotifier() {}
        /*
          Provides the following behavior:
          -Notifies if filename has been modified
          -If filename is deleted, notifies on next creation
        */
        virtual void Notify(const std::string& filename, file_callback callback) = 0;
        virtual void Remove(const std::string& filename, file_callback callback) = 0;
    };
}

#ifdef linux
#include "sore_fileio_linux.h"
#else
#endif


#endif
