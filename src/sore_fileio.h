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
#include <limits>
#include <boost/function.hpp>
#include "sore_gamekernel.h"

namespace SORE_FileIO
{
  typedef size_t file_ref;
  const file_ref PACKAGE_START = 1;
#define FILESYSTEM_START  std::numeric_limits<file_ref>::max()/2
#define FILESYSTEM_END    std::numeric_limits<file_ref>::max()


  typedef boost::function<void (std::string)> file_callback;

  int          SORE_EXPORT InitFileIO(SORE_Kernel::GameKernel* gk); //return 0 on success

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
