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


#pragma warning( push )
#pragma warning( disable : 4995 )

#include <process.h>

#include <sore_defines.h> //for windows.h
#include <sore_fileio.h>
#include <sore_logger.h>

struct thread_info
{
    CRITICAL_SECTION* cs;
    bool* finished;

    HANDLE hDir;
    std::queue<std::string>* notifyEvents;
};

static std::string GetFullName(const std::string& filename)
{
    TCHAR buffer[MAX_PATH];
    GetFullPathName(filename.c_str(), MAX_PATH, buffer, NULL);
    std::string file(buffer);
    return file;
}

static std::string GetParent(const std::string& filename)
{
    std::string file = GetFullName(filename);
    if(file.rfind("\\") == std::string::npos)
        return std::string(".");
    std::string parent = file.substr(0, file.rfind("\\"));
    if(parent.length() == 2 && isalpha(parent[0]) && parent[1] == ':')
    {
        ENGINE_LOG(SORE_Logging::LVL_ERROR, "File watcher does not support opening logical drives");
        return std::string(".");
    }
    return parent;
}

static DWORD WINAPI ThreadFunc(void* args)
{
    thread_info* ti = reinterpret_cast<thread_info*>(args);

    while(true)
    {
        EnterCriticalSection(ti->cs);
        if(*(ti->finished))
        {
            LeaveCriticalSection(ti->cs);
            break;
        }
        LeaveCriticalSection(ti->cs);

        BYTE buffer[32 * 1024];
        DWORD offset = 0;
        DWORD bytes;
        PFILE_NOTIFY_INFORMATION info;
        TCHAR szFile[MAX_PATH];

        ReadDirectoryChangesW(ti->hDir, buffer, 32*1024, 0,
            FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
            &bytes, NULL, NULL);

        do
        {
            info = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(&buffer[offset]);
            offset += info->NextEntryOffset;

#if defined(UNICODE)
            {
                lstrcpynW(szFile, info->FileName,
                    min(MAX_PATH, info->FileNameLength / sizeof(WCHAR) + 1));
            }
#else
            {
                int count = WideCharToMultiByte(CP_ACP, 0, info->FileName,
                    info->FileNameLength / sizeof(WCHAR),
                    szFile, MAX_PATH - 1, NULL, NULL);
                szFile[count] = TEXT('\0');
            }
#endif

            std::string filename(reinterpret_cast<char*>(info->FileName));
            filename = GetFullName(filename);

            ENGINE_LOG(SORE_Logging::LVL_INFO, std::string("change on ") + filename);

        } while(info->NextEntryOffset != 0);
    }
    delete ti;
    return 0;
}

SORE_FileIO::WindowsFileWatcher::WindowsFileWatcher() : finished(false)
{
    InitializeCriticalSection(&synchronization);
}

static void CloseWatchHandle(std::pair<std::string, HANDLE> p)
{
    if(CloseHandle(p.second) == 0)
        ENGINE_LOG(SORE_Logging::LVL_WARNING, "Failed to close file handle");
}

SORE_FileIO::WindowsFileWatcher::~WindowsFileWatcher()
{
    EnterCriticalSection(&synchronization);
    finished = true;
    LeaveCriticalSection(&synchronization);

    std::for_each(watches.begin(), watches.end(), &CloseWatchHandle);

    DeleteCriticalSection(&synchronization);
}

void SORE_FileIO::WindowsFileWatcher::Pause()
{
}

void SORE_FileIO::WindowsFileWatcher::Resume()
{
}

void SORE_FileIO::WindowsFileWatcher::Frame(int elapsedTime)
{
}

SORE_FileIO::notify_handle SORE_FileIO::WindowsFileWatcher::Notify(
    const std::string& filename, file_callback callback)
{
    if(watches.find(GetParent(filename)) == watches.end())
    {
        ENGINE_LOG(SORE_Logging::LVL_INFO, std::string("watching ") + GetParent(filename));
        HANDLE hDir = CreateFile(
            GetParent(filename).c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL);
        if(hDir == INVALID_HANDLE_VALUE)
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Invalid handle received");
        watches.insert(std::make_pair(GetParent(filename), hDir));

        thread_info* ti = new thread_info;
        ti->cs = &synchronization;
        ti->hDir = hDir;
        ti->finished = &finished;
        ti->notifyEvents = &notifyEvents;

        HANDLE hThread = CreateThread(
            NULL,
            0,
            &ThreadFunc, 
            ti,
            0,
            0);

        threads.push_back(hThread);

        return notify_handle();
    }
    else
    {
        return notify_handle();
    }
}

void SORE_FileIO::WindowsFileWatcher::Remove(notify_handle handle)
{
    std::string filename = GetParent(handle.filename);
    /*watchedFiles[filename].callbacks.erase(handle.it);
    if(watchedFiles[filename].callbacks.empty())
        watchedFiles.erase(watchedFiles.find(filename));*/
}

#pragma warning( pop)