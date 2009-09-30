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
 // $Id$

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "sore_fileio.h"
#include "sore_logger.h"

#include <zlib.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define MODULE_FILEIO 1

namespace SORE_FileIO
{
    const size_t CHUNK = 131072;
    const int RATIO = 4;

    struct linked_list
    {
        unsigned char* data;
        unsigned int size;
        linked_list* next;
    };

    void DeleteLinkedList(linked_list* curr);
    void LinkedListCopy(void* dest, linked_list* src, size_t n); //copy n bytes from src to dest
    void LinkedListEat(linked_list* src, size_t n); //shift the data in src n bytes toward the start

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

        size_t       pos;
        size_t       size;
        size_t       sizeRaw;
        size_t       currPos;
        size_t       currPosRaw;
        z_stream*    strm;
        linked_list  out_buf;
        size_t       out_filled;
        size_t       out_size;
    };

    struct filesystem_file
    {
        FILE* fptr;
        size_t filled;
        char buffer[2048];
    };

    typedef std::vector<file_info> file_list;

    file_list cachedFiles;
    std::map<std::string, size_t> fileMap;
    std::map<std::string, FILE*> openPackages;
    std::map<unsigned int, filesystem_file> openFilesystemFiles;
    std::map<std::string, unsigned int> openPackageCount;
    unsigned int nOpenFilesystemFiles;

    file_info* GetFile(unsigned int ID);
    file_info* GetFile(std::string fullname);
    void BuildFullName(file_list& list, file_info& file, file_info& orig);

    const int MAX_MAJOR = 0;
    const int MAX_MINOR = 2;
}

SORE_FileIO::file_info* SORE_FileIO::GetFile(unsigned int ID)
{
    for(file_list::iterator it=cachedFiles.begin(); it!=cachedFiles.end(); ++it)
    {
        if(it->fileID == ID)
            return &*it;
    }
    return 0;
}

SORE_FileIO::file_info* SORE_FileIO::GetFile(std::string fullname)
{
    for(file_list::iterator it=cachedFiles.begin(); it!=cachedFiles.end(); ++it)
    {
        if(it->fullname == fullname)
            return &*it;
    }
    return 0;
}

int SORE_FileIO::InitFileIO(SORE_Kernel::GameKernel* gk)
{
    SORE_Logging::sore_log.IgnoreModule(MODULE_FILEIO,
                                        SORE_Logging::LVL_DEBUG2 | SORE_Logging::LVL_DEBUG3);
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

int SORE_FileIO::CachePackage(const char* package)
{
    int errCode = 0;
    char header[7];

    FILE* in;
    in = fopen(package, "rb");
    if(!in || ferror(in)!=0)
    {
        ENGINE_LOG(SORE_Logging::LVL_ERROR,
                   boost::format("Could not open package file %s, aborting.") % package);
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
        ENGINE_LOG(SORE_Logging::LVL_ERROR,
                   boost::format("Can't decode SDP files greater than version %d.%d")
                   % MAX_MAJOR % MAX_MINOR);
    }

    unsigned short int numPackageFiles = (header[5]) + (header[6]<<8);

    ENGINE_LOG(SORE_Logging::LVL_INFO,
               boost::format("Now processing %s containing %d files and directories...")
               % package % numPackageFiles);

    char flags, c;
    int pos;

    file_info tempInfo;

    //we need to reassign IDs given how many files are already cached so we don't mangle existing
    //ids
    unsigned int idPrefix = cachedFiles.size();

    //we also need to reassign folder IDs if a folder already exists in the cache
    std::map<unsigned int, unsigned int> reassignedIDs;

    for(int i=0;i<numPackageFiles;i++)
    {
        pos = 0;
        fread(&tempInfo.fileID, sizeof(short int), 1, in);
        fread(&flags, sizeof(char), 1, in);
        fread(&tempInfo.parentID, sizeof(short int), 1, in);

        tempInfo.fileID += idPrefix;
        if(tempInfo.parentID == 65535 )
            tempInfo.parentID = 0;
        else
            tempInfo.parentID += idPrefix;

        while((c=fgetc(in))!=(unsigned char)'\0')
        {
            tempInfo.filename[pos] = (unsigned char)c;
            pos++;
        }
        tempInfo.filename[pos] = '\0';

        if(reassignedIDs.find(tempInfo.parentID)!=reassignedIDs.end())
        {
            ENGINE_LOG_M(SORE_Logging::LVL_DEBUG2, boost::format("Reassigning parent for %s to %d")
                         % tempInfo.filename % reassignedIDs[tempInfo.parentID], MODULE_FILEIO);
            tempInfo.parentID = reassignedIDs[tempInfo.parentID];
        }

        if(!(flags & 0x01))
        {
            tempInfo.file = true;
            fread(&tempInfo.pos, sizeof(int), 1, in);
            fread(&tempInfo.size, sizeof(int), 1, in);
            if(flags & 0x02)
                fread(&tempInfo.sizeRaw, sizeof(int), 1, in);
        }
        else
        {
            tempInfo.file = false;
            //look for existing directory of the same name
            BuildFullName(cachedFiles, tempInfo, tempInfo);
            file_info* info = GetFile(tempInfo.fullname);
            if(info)
            {
                reassignedIDs.insert(std::make_pair(tempInfo.fileID, info->fileID));
                continue;
            }
        }
        if(flags & 0x02)
        {
            tempInfo.compressed = true;
        }
        else
        {
            tempInfo.compressed = false;
        }
        tempInfo.isOpen = false;
        strncpy(tempInfo.package, package, 511);
        if(cachedFiles.size()>=FILESYSTEM_START-1)
        {
            //marked as critical because this should never happen, unless you load LOTS of resources
            ENGINE_LOG(SORE_Logging::LVL_CRITICAL, "Too many cached files...aborting");
            fclose(in);
            return -2;
        }
        cachedFiles.push_back(tempInfo);
        BuildFullName(cachedFiles, cachedFiles[cachedFiles.size()-1],
                      cachedFiles[cachedFiles.size()-1]);
        fileMap[cachedFiles[cachedFiles.size()-1].fullname] = cachedFiles.size()-1;
        ENGINE_LOG_M(SORE_Logging::LVL_DEBUG2, boost::format("Added %s to cache")
                     % cachedFiles[cachedFiles.size()-1].fullname, MODULE_FILEIO);
    }

    fclose(in);

    return errCode;
}

int SORE_FileIO::ClosePackage(const char* package)
{
    if(openPackages.find(package)==openPackages.end())
    {
        ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Cannot close %s: package is not open")
                   % package);
        return 0;
    }
    if(openPackageCount.find(package)!=openPackageCount.end() && openPackageCount[package]!=0)
    {
        ENGINE_LOG_M(SORE_Logging::LVL_DEBUG2,
                     boost::format("Closing open files (%u) before closing package")
                     % openPackageCount[package], MODULE_FILEIO);

        file_list::iterator it;

        for(it=cachedFiles.begin();it!=cachedFiles.end();++it)
        {
            if(strcmp(it->package, package) != 0)
                continue;
            if(!it->isOpen)
                continue;

            it->isOpen = false;
            --openPackageCount[package];
            ENGINE_LOG_M(SORE_Logging::LVL_DEBUG2, boost::format("closing %s (%u)")
                         % it->fullname % openPackageCount[package], MODULE_FILEIO);
        }
    }
    if(openPackageCount.find(package)!=openPackageCount.end() && openPackageCount[package]!=0)
    {
        ENGINE_LOG(SORE_Logging::LVL_ERROR,
                   boost::format("Consistency error - incorrect number of open files in %s")
                   % package);
        return 1;
    }
    ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Closing package %s") % package);
    fclose(openPackages[package]);
    openPackages.erase(package);
    return 0;
}

/*
  All file_ref's from packages are in the range 1-2147483647
  All file_ref's from the normal filesystem are in the range 2147483648-4294967295
*/
SORE_FileIO::file_ref SORE_FileIO::Open(const char* file)
{
    std::map<std::string, size_t>::iterator it;
    FILE* temp = NULL;
    temp = fopen(file, "rb");
    if(temp && ferror(temp)==0)
    {
        ENGINE_LOG_M(SORE_Logging::LVL_DEBUG2, boost::format("Opening file %s from disk") % file,
                     MODULE_FILEIO);
        if(nOpenFilesystemFiles>=(unsigned long)FILESYSTEM_END-FILESYSTEM_START)
        {
            ENGINE_LOG(SORE_Logging::LVL_WARNING,"Too many files open, aborting.");
            fclose(temp);
            return 0;
        }

        unsigned int cur = nOpenFilesystemFiles;
        /*
          TODO: Currently, the approach below doesnt completely "free" file_ref's to be used when a
          file is closed - so theoretically, after opening files a couple billion times, we may be
          unable to open any more. This should be made more elegant sometime in the future.
        */
        while(openFilesystemFiles.find(FILESYSTEM_START+cur)!=openFilesystemFiles.end())
            cur++;
        filesystem_file fileStruct;
        fileStruct.fptr = temp;
        fileStruct.filled = 0;
        fileStruct.buffer[0] = '\0';
        openFilesystemFiles[FILESYSTEM_START+cur] = fileStruct;
        nOpenFilesystemFiles++;

        return FILESYSTEM_START+cur;
    }
    else if((it=fileMap.find(file))!=fileMap.end())
    {
        if(cachedFiles[it->second].isOpen)
        {
            ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("%s is already open") % file);
            return it->second;
        }
        cachedFiles[it->second].currPos = 0;
        cachedFiles[it->second].currPosRaw  = 0;
        ENGINE_LOG_M(SORE_Logging::LVL_DEBUG2, boost::format("Opening file %s from package %s")
                     % file % cachedFiles[it->second].package, MODULE_FILEIO);
        if(openPackages.find(cachedFiles[it->second].package)==openPackages.end())
        {
            FILE* temp;
            ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Opening package %s")
                       % cachedFiles[it->second].package);
            temp = fopen(cachedFiles[it->second].package, "rb");
            openPackages[cachedFiles[it->second].package] = temp;
            if(openPackageCount.find(cachedFiles[it->second].package)==openPackageCount.end())
                openPackageCount[cachedFiles[it->second].package] = 1;
            else
                ++openPackageCount[cachedFiles[it->second].package];
        }
        else
        {
            ++openPackageCount[cachedFiles[it->second].package];
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

bool SORE_FileIO::Eof(file_ref file)
{
    if(file<FILESYSTEM_START && file>=PACKAGE_START)
    {
        if(cachedFiles[file].isOpen==false)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "File is not open");
            return true;
        }
        if(cachedFiles[file].currPos==cachedFiles[file].size) return true;
        return false;
    }
    else if(file>=FILESYSTEM_START && file<FILESYSTEM_END)
    {
        if(openFilesystemFiles[file].filled>0) return false;
        if(feof(openFilesystemFiles[file].fptr)!=0) return true;
        else return false;
    }
    return true;
}

void SORE_FileIO::Close(file_ref file)
{
    if(file<FILESYSTEM_START && file>=PACKAGE_START)
    {
        cachedFiles[file].isOpen = false;
        --openPackageCount[cachedFiles[file].package];
        ENGINE_LOG_M(SORE_Logging::LVL_DEBUG2, boost::format("Closing file %s from package cache")
                     % cachedFiles[file].fullname, MODULE_FILEIO);
        if(cachedFiles[file].compressed)
        {
            delete   cachedFiles[file].strm;
            delete[] cachedFiles[file].out_buf.data;
            if(cachedFiles[file].out_buf.next != NULL)
                DeleteLinkedList(cachedFiles[file].out_buf.next);
            cachedFiles[file].out_size = CHUNK * RATIO;
        }
#ifdef SORE_FILEIO_CLOSE_PACKAGE_STRICT
        if(openPackageCount[cachedFiles[file].package]==0)
        {
            ClosePackage(cachedFiles[file].package);
        }
#endif
    }
    else if(file>=FILESYSTEM_START && file<FILESYSTEM_END)
    {
        ENGINE_LOG_M(SORE_Logging::LVL_DEBUG2, boost::format("Closing file reference %u from disk")
                     % file, MODULE_FILEIO);
        nOpenFilesystemFiles--;
        fclose(openFilesystemFiles[file].fptr);
        openFilesystemFiles.erase(file);
    }
}

size_t SORE_FileIO::Read(void *ptr, size_t size, size_t nmemb, file_ref file, bool ignoreBuffer)
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
        //how many bytes we need to read -adjusted from size*nmemb to make sure we don't go past
        //the end of the file in our stream
        size_t bytesToRead = cachedFiles[file].size - cachedFiles[file].currPos;
        size_t read; //how many we've read so far
        fseek(openPackages[cachedFiles[file].package],
              static_cast<long>(cachedFiles[file].pos + cachedFiles[file].currPosRaw), SEEK_SET);
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
            size_t size, ret,have;
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
                    ENGINE_LOG(SORE_Logging::LVL_WARNING,
                               boost::format("\tNeeded to read %d bytes but only got %d bytes.")
                               % size % ret);
                    ENGINE_LOG(SORE_Logging::LVL_WARNING,
                               boost::format("\t%s resides at poisition %d and is %d bytes of size")
                               % cachedFiles[file].filename % cachedFiles[file].pos
                               % cachedFiles[file].size);
                    ENGINE_LOG(SORE_Logging::LVL_WARNING,
                               boost::format("\tCurrent package position is %u. Current file "
                                             "position is %u bytes.")
                               % ftell(openPackages[cachedFiles[file].package])
                               % cachedFiles[file].currPos);
                    return read;
                }
                cachedFiles[file].currPosRaw += static_cast<uInt>(ret);
                cachedFiles[file].strm->avail_in = static_cast<uInt>(ret);
                cachedFiles[file].strm->next_in = in_buf;
                do
                {
                    size_t i = 0;
                    linked_list* curr = &cachedFiles[file].out_buf;
                    while(true)
                    {
                        i += curr->size;
                        if(i >= cachedFiles[file].out_filled)
                            break;
                        assert(curr->next != NULL);
                        curr = curr->next;
                    }
                    size_t out;
                    if(cachedFiles[file].out_size - cachedFiles[file].out_filled >= CHUNK)
                        out = CHUNK;
                    else if(cachedFiles[file].out_size - cachedFiles[file].out_filled < CHUNK &&
                            cachedFiles[file].out_size - cachedFiles[file].out_filled > 0)
                        out = cachedFiles[file].out_size-cachedFiles[file].out_filled;
                    else
                    {
                        curr->next = new linked_list;
                        curr->next->data = new unsigned char[CHUNK*RATIO];
                        curr->next->size = CHUNK * RATIO;
                        curr->next->next = NULL;
                        cachedFiles[file].out_size += CHUNK * RATIO;
                        out = CHUNK;
                        curr = curr->next;
                    }
                    cachedFiles[file].strm->avail_out = static_cast<uInt>(out);
                    size_t currFilled = cachedFiles[file].out_filled % (CHUNK*RATIO);
                    cachedFiles[file].strm->next_out = (curr->data + currFilled);
                    int inflated = inflate(cachedFiles[file].strm, Z_NO_FLUSH);
                    assert(inflated != Z_STREAM_ERROR);
                    switch (inflated)
                    {
                        case Z_NEED_DICT:
                            inflated = Z_DATA_ERROR;     //and fall through
                        case Z_DATA_ERROR:
                        case Z_MEM_ERROR:
                            inflateEnd(cachedFiles[file].strm);
                            return read;
                    }
                    have = out - static_cast<size_t>(cachedFiles[file].strm->avail_out);
                    cachedFiles[file].out_filled += have;

                }while(cachedFiles[file].strm->avail_out == 0);
                if(cachedFiles[file].out_filled >= bytesToRead-read)
                {
                    LinkedListCopy((unsigned char*)ptr+read, &cachedFiles[file].out_buf,
                                   bytesToRead-read);
                    LinkedListEat(&cachedFiles[file].out_buf, bytesToRead-read);
                    cachedFiles[file].out_size -= bytesToRead / (CHUNK*RATIO);
                    cachedFiles[file].out_filled -= bytesToRead - read;
                    cachedFiles[file].currPos += bytesToRead - read;
                    read = bytesToRead;

                }
                else
                {
                    LinkedListCopy((unsigned char*)ptr+read, &cachedFiles[file].out_buf,
                                   cachedFiles[file].out_filled);
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
        size_t offset = 0;
        size_t toRead = size*nmemb;
        if(!ignoreBuffer)
        {
            if(openFilesystemFiles[file].filled>0)
            {
                size_t amount = size*nmemb > openFilesystemFiles[file].filled ?
                    openFilesystemFiles[file].filled : size*nmemb;
                memcpy(ptr, openFilesystemFiles[file].buffer, amount);
                offset = amount;
                toRead -= amount;
                openFilesystemFiles[file].filled -= amount;
                openFilesystemFiles[file].buffer[0] = '\0';
            }
        }
        size_t read = fread((char*)ptr+offset, 1, toRead, openFilesystemFiles[file].fptr);
        if(read+offset != size*nmemb)
        {
            unsigned int errLvl = SORE_Logging::LVL_DEBUG2;
            if(ferror(openFilesystemFiles[file].fptr)!=0)
            {
                errLvl = SORE_Logging::LVL_ERROR;
            }
            ENGINE_LOG_M(errLvl, boost::format("Could not read all %u bytes: ")
                         % (static_cast<unsigned int>(size*nmemb)), MODULE_FILEIO);
            ENGINE_LOG_M(errLvl, boost::format("ferror: %s, feof: %s")
                         % ferror(openFilesystemFiles[file].fptr)
                         % feof(openFilesystemFiles[file].fptr), MODULE_FILEIO);
        }
        return read+offset;
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

size_t SORE_FileIO::Read(char* ptr, size_t num, const char* separator, file_ref file)
{
    assert(num<=2048 && "Trying to read too many characters");
    char* data = openFilesystemFiles[file].buffer;
    size_t length = openFilesystemFiles[file].filled;
    if(*data=='\0')
    {
        size_t len = Read(data, sizeof(char), num, file,true);
        data[length+len] = '\0';
        int stop = strpos((char*)data,(char*)separator);
        if(stop==-1)
        {
            memcpy(ptr, data, len);
            ptr[len] = '\0';
            *data = '\0';
            openFilesystemFiles[file].filled = 0;
            return len;
        }
        else
        {
            memcpy(ptr, data, stop);
            ptr[stop] = '\0';
            openFilesystemFiles[file].filled = len - stop - 1;
            memmove(data, data+stop+1, openFilesystemFiles[file].filled);
            data[openFilesystemFiles[file].filled] = '\0';
            return stop;
        }
    }
    else
    {
        size_t len = Read(data+length, sizeof(char), num-length, file,true);
        data[length+len] = '\0';
        int stop = strpos((char*)data,(char*)separator);
        if(stop==-1)
        {
            memcpy(ptr, data, len+length);
            ptr[len+length] = '\0';
            *data = '\0';
            len+=length;
            openFilesystemFiles[file].filled = 0;
            return len;
        }
        else
        {
            memcpy(ptr, data, stop);
            ptr[stop] = '\0';
            openFilesystemFiles[file].filled = length - stop + len - 1;
            memmove(data, data+stop+1, openFilesystemFiles[file].filled+1);
            return stop;
        }
    }
    return 0;
}

void SORE_FileIO::BuildFullName(file_list& list, file_info& file, file_info& orig)
{
    if(file.parentID!=0)
    {
        BuildFullName(list, *GetFile(file.parentID), orig);
        strcat(orig.fullname, file.filename);
    }
    else
    {
        strcpy(orig.fullname, file.filename);
    }
    if(!file.file)
        strcat(orig.fullname, "/");
}

void SORE_FileIO::DeleteLinkedList(linked_list* curr)
{
    if(curr->next!=NULL)
        DeleteLinkedList(curr->next);
    delete curr;
}

//copy n bytes from src to dest
void SORE_FileIO::LinkedListCopy(void* dest, linked_list* src, size_t n)
{
    size_t copied = 0;
    linked_list* curr = src;
    while(copied < n)
    {
        assert(curr != NULL);
        size_t toCopy;
        if(n - copied > curr->size)
            toCopy = curr->size;
        else
            toCopy = n - copied;
        memcpy((unsigned char*)dest+copied, curr->data, toCopy);
        copied += toCopy;
        curr = curr->next;
    }
}

//shift the data in src n bytes toward the start
void SORE_FileIO::LinkedListEat(linked_list* src, size_t n)
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

    for(size_t i=0;i<curr->size-n;i++)
    {
        curr->data[i] = curr->data[i+n];
    }
    while(curr->next!=NULL)
    {
        for(size_t i=0;i<n;i++)
        {
            curr->data[i + (curr->size - n)] = curr->next->data[i];
        }
        curr = curr->next;
        for(size_t i=0;i<curr->size-n;i++)
        {
            curr->data[i] = curr->data[i+n];
        }
    }
}

size_t SORE_FileIO::Size(file_ref file)
{
    if(file >= FILESYSTEM_START)
    {
        unsigned int currPos = ftell(openFilesystemFiles[file].fptr);
        fseek(openFilesystemFiles[file].fptr, 0, SEEK_END);
        unsigned int len = ftell(openFilesystemFiles[file].fptr);
        fseek(openFilesystemFiles[file].fptr, currPos, SEEK_SET);
        return len;
    }
    else
        return cachedFiles[file].size;
}

size_t SORE_FileIO::CompressedSize(file_ref file)
{
    if(file > FILESYSTEM_START || !cachedFiles[file].compressed)
        return Size(file);
    return cachedFiles[file].sizeRaw;
}

#undef MODULE_FILEIO
