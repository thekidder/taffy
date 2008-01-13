// $Id$

#ifndef  __SORE_FILEIO__
#define  __SORE_FILEIO__

#include <cstdio>
#include <limits>
#include <boost/function.hpp>
#include "sore_kernel.h"

namespace SORE_FileIO
{
	const unsigned long PACKAGE_START = 1;
	const unsigned long FILESYSTEM_START = std::numeric_limits<unsigned long>::max()/2;
	const unsigned long FILESYSTEM_END   = std::numeric_limits<unsigned long>::max();
	
	typedef unsigned long file_ref;
	typedef boost::function<void (std::string)> file_callback;
	
	int          InitFileIO(SORE_Kernel::GameKernel* gk); //return 0 on success
	
	/*
	  Call this to include a package and make its contents available to the application
	  If filenames in two or more packages are duplicated, the last one included will always be used
	*/
	int          CachePackageInfo(const char* package); //return 0 on success
	
	file_ref     Open(const char* file); //return a file_ref on success, 0 on failure
	void         Close(file_ref file);

	int          Read(void* ptr, size_t size, size_t nmemb, file_ref file);    // returns number of bytes read
	int          Read(char* ptr, size_t num, const char* separator, file_ref file);           // reads until num number of bytes or until any character in separator has been reached
	unsigned int Size(file_ref file);
	unsigned int CompressedSize(file_ref file);
	
	void         Notify(std::string filename, file_callback callback); //only implemented on local files now (no packages)
	
	bool         InitFileNotify(SORE_Kernel::GameKernel* gk); //called by InitFileIO, no need to ever call this yourself
}

#endif /*__SORE_FILEIO__*/
