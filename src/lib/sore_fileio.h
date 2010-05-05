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
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
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

#ifndef  SORE_FILEIO_H
#define  SORE_FILEIO_H

#include <cstdio>
#include <iosfwd>
#include <limits>
#include <list>

#include <boost/function.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <zlib.h>

#include <sore_gamekernel.h>
#include <sore_noncopyable.h>
#include <sore_task.h>

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

            size_t             pos;
            size_t             size;
            size_t             sizeRaw;
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

    class SORE_EXPORT InFile : SORE_Utility::Noncopyable
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

    typedef boost::function<void (const std::string&)> file_callback;

#ifdef linux
    class LinuxInotifyWatcher;

    struct notify_handle
    {
        notify_handle() {}
        notify_handle(
            const std::string& file,
            const std::list<std::pair<std::string, file_callback> >::iterator& i)
            : filename(file), it(i) {}
    private:
        friend class LinuxInotifyWatcher;
        std::string filename;
        std::list<std::pair<std::string, file_callback> >::iterator it;
    };

#else
    struct notify_handle
	{
		notify_handle() {}
	private:
        friend class LinuxInotifyWatcher;
	};
#endif

    class FilesystemNotifier : public SORE_Kernel::Task
    {
    public:
        virtual ~FilesystemNotifier() {}
        /*
          Provides the following behavior:
          -Notifies if filename has been modified
          -If filename is deleted, notifies on next creation
        */
        virtual notify_handle Notify(
            const std::string& filename, file_callback callback) = 0;
        virtual void Remove(notify_handle handle) = 0;
    };
}

#ifdef linux
#include <sore_fileio_linux.h>
#else
#include <sore_fileio_windows.h>
#endif


#endif
