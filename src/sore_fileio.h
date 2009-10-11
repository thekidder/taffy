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

#include "sore_gamekernel.h"

namespace SORE_FileIO
{
    class InFile
    {
    public:
        virtual std::ostream& strm() = 0;
        virtual std::size_t size() = 0;
    };

    class PackageCache
    {
    public:
        PackageCache();
        ~PackageCache();

        void AddPackage(const char* packagename);

        bool Contains(const char* filename) const;
        InFile* GetFile(const char* filename);
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
        std::ofstream& GetPackage(const char* packagename);

        //all cached files and where they are
        //stored as a (fullname, file_info) pair
        cache_type cache;
        //which packages have currently been cached
        std::vector<std::string> cachedPackages;
        //open file handles
        std::map<std::string, std::ifstream> openPackages;
    };

    class CompressedPkgFileBuf : public boost::iostreams::source
    {
    public:
        CompressedPkgFileBuf();
        std::streamsize read(char* s, std::streamsize n);
    private:
    };

  /*
    Call this to include a package and make its contents available to the application
    If filenames in two or more packages are duplicated, the last one included will
    always be used. Returns 0 on success.
  */
  int          SORE_EXPORT CachePackage(const char* package);
  /*
    Call this to explicitly close a package. Package cache is still available to the
    application, but the file handle is not open. Returns 0 on success.
  */
  int          SORE_EXPORT ClosePackage(const char* package);

  file_ref     SORE_EXPORT Open(const char* file); //return a file_ref on success, 0 on failure
  void         SORE_EXPORT Close(file_ref file);

// returns number of bytes read
  size_t       SORE_EXPORT Read(void* ptr, size_t size, size_t nmemb, file_ref file,
                                bool ignoreBuffer=false);
// reads until num number of bytes or until any character in separator has been reached
  size_t       SORE_EXPORT Read(char* ptr, size_t num, const char* separator, file_ref file);
  size_t       SORE_EXPORT Size(file_ref file);
  size_t       SORE_EXPORT CompressedSize(file_ref file);
  bool         SORE_EXPORT Eof(file_ref file);

//only implemented on local files now (no packages)
  void         SORE_EXPORT Notify(std::string filename, file_callback callback);
//called by InitFileIO, no need to ever call this yourself
  bool         SORE_EXPORT InitFileNotify(SORE_Kernel::GameKernel* gk);
}

#endif
