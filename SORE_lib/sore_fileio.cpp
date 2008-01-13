
#include "sore_fileio.h"
#include "sore_logger.h"

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <zlib.h>
#include <cassert>

namespace SORE_FileIO
{
	const int CHUNK = 131072;
	const int RATIO = 4;
	
	struct linked_list
	{
		unsigned char* data;
		unsigned int size;
		linked_list* next;
	};
	
	void DeleteLinkedList(linked_list* curr);
	void LinkedListCopy(void* dest, linked_list* src, unsigned int n); //copy n bytes from src to dest
	void LinkedListEat(linked_list* src, unsigned int n); //shift the data in src n bytes toward the start
	
	struct file_info
	{
		unsigned short int fileID;
		unsigned short int parentID;
		bool               file;
		bool               isOpen;
		bool               compressed;
	
		char               filename[256];
		char               fullname[512];
		char               package [512];
	
		unsigned int       pos;
		unsigned int       size;
		unsigned int       sizeRaw;
		unsigned int       currPos;
		unsigned int       currPosRaw;
		z_stream*          strm;
		linked_list        out_buf;
		unsigned int       out_filled;
		unsigned int       out_size;
	};

	typedef std::vector<file_info> file_list;
	
	file_list cachedFiles;
	std::map<std::string, int> fileMap;
	std::map<std::string, FILE*> openPackages;
	std::map<unsigned int, FILE*> openFilesystemFiles;
	std::map<std::string, unsigned int> openPackageCount;
	unsigned int nOpenFilesystemFiles;
	
	
	void BuildFullName(file_list& list, file_info& file, file_info& orig);

	const int MAX_MAJOR = 0;
	const int MAX_MINOR = 2;
	
}

int SORE_FileIO::InitFileIO(SORE_Kernel::GameKernel* gk)
{
	file_info temp;
	cachedFiles.clear();
	cachedFiles.push_back(temp);
	fileMap.clear();
	openPackages.clear();
	openPackageCount.clear();
	openFilesystemFiles.clear();
	nOpenFilesystemFiles = 0;
	if(!InitFileNotify(gk)) return 1;
	return 0;
}

int SORE_FileIO::CachePackageInfo(const char* package)
{
	int errCode = 0;
	char header[7];
	
	FILE* in;
	in = fopen(package, "rb");
	if(!in || ferror(in)!=0)
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Could not open package file %s, aborting.") % package);
		//SORE_Logging::sore_log.Log(SORE_Logging::LVL_ERROR, __LINE__, __PRETTY_FUNCTION__, __FILE__, "Could not open package file %s, aborting.", package);
		return -1;
	}
	fread(header, sizeof(char), 7, in);
	
	if(header[0]!='S' || header[1]!='D' || header[2]!='P')
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, "Not a SORE Data Package file");
		return -1;
	}
	
	if(int(header[3])>MAX_MAJOR || int(header[4])>MAX_MINOR)
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Can't decode SDP files greater than version %d.%d") % MAX_MAJOR % MAX_MINOR);
	}
	
	unsigned short int numPackageFiles = (header[5]) + (header[6]<<8);
	
	ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Now processing %s containing %d files and directories...") % package % numPackageFiles);
		
	char flags, c;
	int pos;
	
	file_info tempInfo;
	
	for(int i=0;i<numPackageFiles;i++)
	{
		pos = 0;
		fread(&tempInfo.fileID, sizeof(short int), 1, in); 
		fread(&flags, sizeof(char), 1, in); 
		fread(&tempInfo.parentID, sizeof(short int), 1, in);
		while((c=fgetc(in))!=(unsigned char)'\0')
		{
			tempInfo.filename[pos] = (unsigned char)c;
			pos++;
		}
		tempInfo.filename[pos] = '\0';
		
		if(!(flags & 0x01))
		{
			tempInfo.file = true;
			fread(&tempInfo.pos, sizeof(int), 1, in); 
			fread(&tempInfo.size, sizeof(int), 1, in); 
			if(flags & 0x02)
				fread(&tempInfo.sizeRaw, sizeof(int), 1, in);
		}
		else
			tempInfo.file = false;
		if(flags & 0x02)
		{
			tempInfo.compressed = true;
		}
		else
			tempInfo.compressed = false;
		tempInfo.isOpen = false;
		strcpy(tempInfo.package, package);
		if(cachedFiles.size()>=FILESYSTEM_START-1)
		{
			ENGINE_LOG(SORE_Logging::LVL_CRITICAL, "Too many cached files...aborting"); //marked as critical because this should never happen...unless you load LOTS of resources
			fclose(in);
			return -2;
		}
		cachedFiles.push_back(tempInfo);
		BuildFullName(cachedFiles, cachedFiles[cachedFiles.size()-1], cachedFiles[cachedFiles.size()-1]);
		ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Adding %s to cache") % cachedFiles[cachedFiles.size()-1].fullname);
		fileMap[cachedFiles[cachedFiles.size()-1].fullname] = cachedFiles.size()-1;
	}
	
	fclose(in);

	return errCode;
}

/*
  All file_ref's from packages are in the range 1-2147483647
  All file_ref's from the normal filesystem are in the range 2147483648-4294967295
*/

SORE_FileIO::file_ref SORE_FileIO::Open(const char* file)
{
	std::map<std::string, int>::iterator it;
	FILE* temp = NULL;
	temp = fopen(file, "rb");
	if(temp && ferror(temp)==0)
	{
		//fclose(temp);
		ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Opening file %s from disk") % file);
		if(nOpenFilesystemFiles>=(unsigned long)FILESYSTEM_END-FILESYSTEM_START)
		{
			ENGINE_LOG(SORE_Logging::LVL_WARNING,"Too many files open, aborting.");
			fclose(temp);
			return 0;
		}
			
		//FILE* temp;
		//temp = fopen(file, "rb");
		unsigned int cur = nOpenFilesystemFiles;
			/*TODO:
		Currently, the approach below doesnt completely "free" file_ref's to be used when a file is closed - so theoretically, after opening files a couple billion times, we may be unable to open any more. This should be made more elegant sometime in the future.
			*/
		while(openFilesystemFiles.find(FILESYSTEM_START+cur)!=openFilesystemFiles.end())
			cur++;
		openFilesystemFiles[FILESYSTEM_START+cur] = temp;
		nOpenFilesystemFiles++;
		return FILESYSTEM_START+cur;
	}
	else if((it=fileMap.find(file))!=fileMap.end())
	{
		if(cachedFiles[it->second].isOpen)
		{
			ENGINE_LOG(SORE_Logging::LVL_WARNING,"This file is already open, aborting.");
			return 0;
		}
		cachedFiles[it->second].currPos = 0;
		cachedFiles[it->second].currPosRaw  = 0;
		ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Opening file %s from package %s") % file % cachedFiles[it->second].package);
		if(openPackages.find(cachedFiles[it->second].package)==openPackages.end())
		{
			FILE* temp;
			ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Opening package %s") % cachedFiles[it->second].package);
			temp = fopen(cachedFiles[it->second].package, "rb");
			openPackages[cachedFiles[it->second].package] = temp;
			if(openPackageCount.find(cachedFiles[it->second].package)==openPackageCount.end())
				openPackageCount[cachedFiles[it->second].package] = 1;
			else
				openPackageCount[cachedFiles[it->second].package]++;
		}
		cachedFiles[it->second].isOpen = true;
		if(cachedFiles[it->second].compressed)
		{
			cachedFiles[it->second].out_buf.data = new unsigned char[CHUNK*RATIO];
			cachedFiles[it->second].out_buf.next = NULL;
			cachedFiles[it->second].out_buf.size = CHUNK * RATIO;
			cachedFiles[it->second].out_filled  = 0;
			
			cachedFiles[it->second].out_size    = CHUNK * RATIO;
			int ret;
			cachedFiles[it->second].strm           = new z_stream;
			cachedFiles[it->second].strm->zalloc   = Z_NULL;
			cachedFiles[it->second].strm->zfree    = Z_NULL;
			cachedFiles[it->second].strm->opaque   = Z_NULL;
			cachedFiles[it->second].strm->avail_in = 0;
			cachedFiles[it->second].strm->next_in  = Z_NULL;
			ret = inflateInit(cachedFiles[it->second].strm);
			if (ret != Z_OK)
				return 0;
		}
		return it->second;
	}
	else
	{
		if(temp) fclose(temp);
		return 0;
	}
}

void SORE_FileIO::Close(file_ref file)
{
	if(file<FILESYSTEM_START && file>=PACKAGE_START)
	{
		ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Closing file %s from package cache") % cachedFiles[file].filename);
		cachedFiles[file].isOpen = false;
		if(cachedFiles[file].compressed)
		{
			delete   cachedFiles[file].strm;
			delete[] cachedFiles[file].out_buf.data;
			if(cachedFiles[file].out_buf.next != NULL)
				DeleteLinkedList(cachedFiles[file].out_buf.next);
			cachedFiles[file].out_size = CHUNK * RATIO;
		}
		if(--openPackageCount[cachedFiles[file].package]==0)
		{
			ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Closing package %s") % cachedFiles[file].package);
			fclose(openPackages[cachedFiles[file].package]);
			openPackages.erase(cachedFiles[file].package);
		}
	}
	else if(file>=FILESYSTEM_START && file<FILESYSTEM_END)
	{
		ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Closing file reference %u from disk") % file);
		nOpenFilesystemFiles--;
		fclose(openFilesystemFiles[file]);
		openFilesystemFiles.erase(file);
	}
}

int SORE_FileIO::Read(void *ptr, size_t size, size_t nmemb, file_ref file)
{
	if(nmemb == 0 || size == 0)
		return 0;
	if(file < FILESYSTEM_START && file>=PACKAGE_START)
	{
		if(cachedFiles[file].isOpen==false)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "File is not open");
			return 0;
		}
		unsigned int bytesToRead = cachedFiles[file].size - cachedFiles[file].currPos; //how many bytes we need to read -adjusted from size*nmemb to make sure we don't go past the end of the file in our stream
		unsigned int read; //how many we've read so far
		fseek(openPackages[cachedFiles[file].package], cachedFiles[file].pos + cachedFiles[file].currPosRaw, SEEK_SET);
		if(bytesToRead>=size*nmemb)
		{
			bytesToRead = nmemb;
		}
		else
		{
			bytesToRead /= size;
		}
		if(cachedFiles[file].compressed)
		{
			int ret, size, have;
			unsigned char in_buf[CHUNK];
			read = 0;
			if(cachedFiles[file].out_filled > bytesToRead)
			{
				LinkedListCopy(ptr, &cachedFiles[file].out_buf, bytesToRead);
				LinkedListEat(&cachedFiles[file].out_buf, bytesToRead);
				cachedFiles[file].out_filled -= bytesToRead;
				read += bytesToRead;
				cachedFiles[file].currPos += read;
			}
			else if(cachedFiles[file].out_filled > 0)
			{
				LinkedListCopy(ptr, &cachedFiles[file].out_buf, cachedFiles[file].out_filled);
				read += cachedFiles[file].out_filled;
				cachedFiles[file].out_filled = 0;
				if(cachedFiles[file].out_buf.next != NULL)
					DeleteLinkedList(cachedFiles[file].out_buf.next);
				cachedFiles[file].out_size = CHUNK * RATIO;
				cachedFiles[file].currPos += read;
			}
			while(read < bytesToRead)
			{
				if(CHUNK > cachedFiles[file].sizeRaw - cachedFiles[file].currPosRaw)
					size = cachedFiles[file].sizeRaw - cachedFiles[file].currPosRaw;
				else
					size = CHUNK;
				if((ret=fread(in_buf, 1, size, openPackages[cachedFiles[file].package]))!=size)
				{
					cachedFiles[file].currPos += ret;
					ENGINE_LOG(SORE_Logging::LVL_WARNING, "Reading from file failed:");
					ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("\tNeeded to read %d bytes but only got %d bytes.") % size % ret);
					ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("\t %s resides at poisition %d and is %d bytes of size") % cachedFiles[file].filename % cachedFiles[file].pos % cachedFiles[file].size);
					ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("\tCurrent package position is %u. Current file position is %u bytes.") % ftell(openPackages[cachedFiles[file].package]) % cachedFiles[file].currPos);
					return read;
				}
				cachedFiles[file].currPosRaw += ret;
				cachedFiles[file].strm->avail_in = ret;
				cachedFiles[file].strm->next_in = in_buf;
				do
				{
					int i = 0;
					linked_list* curr = &cachedFiles[file].out_buf;
					while(true)
					{
						i += curr->size;
						if(i >= cachedFiles[file].out_filled)
							break;
						assert(curr->next != NULL);
						curr = curr->next;
					}
					int out;
					if(cachedFiles[file].out_size - cachedFiles[file].out_filled >= CHUNK)
						out = CHUNK;
					else if(cachedFiles[file].out_size - cachedFiles[file].out_filled < CHUNK && cachedFiles[file].out_size - cachedFiles[file].out_filled > 0)
						out = cachedFiles[file].out_size-cachedFiles[file].out_filled;
					else
					{
						unsigned int size = 0;
						
						curr->next = new linked_list;
						curr->next->data = new unsigned char[CHUNK*RATIO];
						curr->next->size = CHUNK * RATIO;
						curr->next->next = NULL;
						cachedFiles[file].out_size += CHUNK * RATIO;
						out = CHUNK;
						curr = curr->next;
					}
					cachedFiles[file].strm->avail_out = out;
					unsigned int currFilled = cachedFiles[file].out_filled % (CHUNK*RATIO);
					cachedFiles[file].strm->next_out = (curr->data + currFilled);
					ret = inflate(cachedFiles[file].strm, Z_NO_FLUSH);
					assert(ret != Z_STREAM_ERROR);
					switch (ret) 
					{
						case Z_NEED_DICT:
							ret = Z_DATA_ERROR;     //and fall through
						case Z_DATA_ERROR:
						case Z_MEM_ERROR:
							inflateEnd(cachedFiles[file].strm);
							return read;
					}
					have = out - cachedFiles[file].strm->avail_out;
					cachedFiles[file].out_filled += have;
					
				}while(cachedFiles[file].strm->avail_out == 0);
				if(cachedFiles[file].out_filled >= bytesToRead-read)
				{
					LinkedListCopy((unsigned char*)ptr+read, &cachedFiles[file].out_buf, bytesToRead-read);
					LinkedListEat(&cachedFiles[file].out_buf, bytesToRead-read);
					cachedFiles[file].out_size -= bytesToRead / (CHUNK*RATIO);
					cachedFiles[file].out_filled -= bytesToRead - read;
					cachedFiles[file].currPos += bytesToRead - read;
					read = bytesToRead;
					
				}
				else
				{
					LinkedListCopy((unsigned char*)ptr+read, &cachedFiles[file].out_buf, cachedFiles[file].out_filled);
					read += cachedFiles[file].out_filled;
					cachedFiles[file].currPos += cachedFiles[file].out_filled;
					cachedFiles[file].out_filled = 0;
					if(cachedFiles[file].out_buf.next != NULL)
						DeleteLinkedList(cachedFiles[file].out_buf.next);
					cachedFiles[file].out_size = CHUNK * RATIO;
				}
			}
			return read;
		}
		else
		{
			read = fread(ptr, size, bytesToRead, openPackages[cachedFiles[file].package]);
		}
		cachedFiles[file].currPosRaw += read;
		return read;
	}
	else if(file>=FILESYSTEM_START && file<std::numeric_limits<unsigned long>::max())
	{
		int read = fread(ptr, size, nmemb, openFilesystemFiles[file]);
		if(read != size*nmemb)
		{
			unsigned int errLvl = SORE_Logging::LVL_DEBUG1;
			if(ferror(openFilesystemFiles[file])!=0)
			{
				errLvl = SORE_Logging::LVL_ERROR;
			}
			ENGINE_LOG(errLvl, boost::format("Could not read all %d bytes: ") % (size*nmemb));
			ENGINE_LOG(errLvl, boost::format("ferror: %s, feof: %s") % ferror(openFilesystemFiles[file]) % feof(openFilesystemFiles[file]));
		}
		return read;
	}
	else
		return 0;
}

int strpos(char* str, char* chars)
{
	int len = 0;
	for(char* i=str;*i!='\0';i++,len++)
	{
		for(char* j=chars;*j!='\0';j++)
		{
			if(*j==*i) return len;
		}
	}
	return -1;
}

int SORE_FileIO::Read(char* ptr, size_t num, const char* separator, file_ref file)
{
	assert(num<=64 && "Trying to read too many characters");
	static char data[64]="";
	static int length = 0;
	if(*data=='\0')
	{
		int len = Read(data, sizeof(char), num, file);
		data[length+len] = '\0';
		int stop = strpos((char*)data,(char*)separator);
		if(stop==-1)
		{
			memcpy(ptr, data, len);
			ptr[len] = '\0';
			*data = '\0';
			length = 0;
			return len;
		}
		else
		{
			memcpy(ptr, data, stop);
			ptr[stop] = '\0';
			length = len - stop -1;
			memcpy(data, data+stop+1, length+len);
			data[length] = '\0';
			return stop;
		}
	}
	else
	{
		int len = Read(data+length, sizeof(char), num-length, file);
		data[length+len] = '\0';
		int stop = strpos((char*)data,(char*)separator);
		if(stop==-1)
		{
			memcpy(ptr, data, len+length);
			ptr[len+length] = '\0';
			*data = '\0';
			len+=length;
			length = 0;
			return len;
		}
		else
		{
			memcpy(ptr, data, stop);
			ptr[stop] = '\0';
			length = length - stop + len;
			memcpy(data, data+stop+1, length);
			//data[length] = '\0';
			return stop;
		}
	}
	return 0;
}

void SORE_FileIO::BuildFullName(file_list& list, file_info& file, file_info& orig)
{
	if(file.parentID!=65535)
	{
		BuildFullName(list, list[file.parentID], orig);
		strcat(orig.fullname, file.filename);
	}
	else
	{
		strcpy(orig.fullname, file.filename);
	}		
	if(!file.file)
		strcat(orig.fullname, "/");
	//return 0;
}

void SORE_FileIO::DeleteLinkedList(linked_list* curr)
{
	if(curr->next!=NULL)
		DeleteLinkedList(curr->next);
	/*if(curr->size==1)
		delete curr->data;
	else
		delete[] curr->data;*/
	delete curr;
}

void SORE_FileIO::LinkedListCopy(void* dest, linked_list* src, unsigned int n)//copy n bytes from src to dest
{
	unsigned int copied = 0;
	linked_list* curr = src;
	while(copied < n)
	{
		assert(curr != NULL);
		unsigned int toCopy;
		if(n - copied > curr->size)
			toCopy = curr->size;
		else
			toCopy = n - copied;
		memcpy((unsigned char*)dest+copied, curr->data, toCopy);
		copied += toCopy;
		curr = curr->next;
	}
}

void SORE_FileIO::LinkedListEat(linked_list* src, unsigned int n) //shift the data in src n bytes toward the start
{
	linked_list* curr = src;
	while(n > curr->size)
	{
		n  -= curr->size;
		linked_list * temp = curr;
		while(temp->next != NULL)
		{
			memcpy(temp->data, temp->next->data, temp->size);
			temp = temp->next;
		}
	}

	for(int i=0;i<curr->size-n;i++)
	{
		curr->data[i] = curr->data[i+n];
	}
	while(curr->next!=NULL)
	{
		for(int i=0;i<n;i++)
		{
			curr->data[i + (curr->size - n)] = curr->next->data[i];
		}
		curr = curr->next;
		for(int i=0;i<curr->size-n;i++)
		{
			curr->data[i] = curr->data[i+n];
		}
	}
}

unsigned int SORE_FileIO::Size(file_ref file)
{
	if(file >= FILESYSTEM_START)
	{
		unsigned int currPos = ftell(openFilesystemFiles[file]);
		fseek(openFilesystemFiles[file], 0, SEEK_END);
		unsigned int len = ftell(openFilesystemFiles[file]);
		fseek(openFilesystemFiles[file], currPos, SEEK_SET);
		return len;
	}
	else
		return cachedFiles[file].size;
}

unsigned int SORE_FileIO::CompressedSize(file_ref file)
{
	if(file > FILESYSTEM_START || !cachedFiles[file].compressed)
		return Size(file);
	return cachedFiles[file].sizeRaw;
}
