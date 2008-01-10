
#ifndef  __SORE_FILEIO__
#define  __SORE_FILEIO__

#include  <cstdio>
#include <limits>

namespace SORE_FileIO
{
	const unsigned long PACKAGE_START = 1;
	const unsigned long FILESYSTEM_START = std::numeric_limits<unsigned long>::max()/2;
	const unsigned long FILESYSTEM_END   = std::numeric_limits<unsigned long>::max();
	
	typedef unsigned long file_ref;
	
	int          InitFileIO(); //return 0 on success
	
	/*
	  Call this to include a package and make its contents available to the application
	  If filenames in two or more packages are duplicated, the last one included will always be used
	*/
	int          CachePackageInfo(const char* package); //return 0 on success
	
	file_ref     Open(const char* file); //return a file_ref on success, 0 on failure
	void         Close(file_ref file);

	int          Read(void *ptr, size_t size, size_t nmemb, file_ref file);    // returns number of bytes read
	unsigned int Size(file_ref file);
	unsigned int CompressedSize(file_ref file);
	
}

#endif /*__SORE_FILEIO__*/
