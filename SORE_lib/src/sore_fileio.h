// $Id$

#ifndef  __SORE_FILEIO__
#define  __SORE_FILEIO__

#include <cstdio>
#include <limits>
#include <boost/function.hpp>
#include "sore_kernel.h"

namespace SORE_FileIO
{
  typedef size_t file_ref;
  const file_ref PACKAGE_START = 1;
#define FILESYSTEM_START  std::numeric_limits<file_ref>::max()/2
#define FILESYSTEM_END    std::numeric_limits<file_ref>::max()
	
	
  typedef boost::function<void (std::string)> file_callback;
	
  int          InitFileIO(SORE_Kernel::GameKernel* gk); //return 0 on success
	
  /*
    Call this to include a package and make its contents available to the application
    If filenames in two or more packages are duplicated, the last one included will always be used
  */
  int          CachePackageInfo(const char* package); //return 0 on success
	
  file_ref     Open(const char* file); //return a file_ref on success, 0 on failure
  void         Close(file_ref file);

  size_t       Read(void* ptr, size_t size, size_t nmemb, file_ref file, bool ignoreBuffer=false);    // returns number of bytes read
  size_t       Read(char* ptr, size_t num, const char* separator, file_ref file);           // reads until num number of bytes or until any character in separator has been reached
  size_t       Size(file_ref file);
  size_t       CompressedSize(file_ref file);
  bool         Eof(file_ref file);
	
  void         Notify(std::string filename, file_callback callback); //only implemented on local files now (no packages)
  bool         InitFileNotify(SORE_Kernel::GameKernel* gk); //called by InitFileIO, no need to ever call this yourself
	
  /*void         ClearDirectory(const char* dir); //delete all files in a directory
    void         CreateDirectory(const char* dir);
    bool         PathExists(const char* path);
    bool         DirectoryExists(const char* dir); //returns true if dir exists and is a directory*/
}

#endif /*__SORE_FILEIO__*/
