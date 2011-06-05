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
* THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
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

#define _WIN32_WINNT 0x0400
#include <sore_defines.h> //for windows.h

#include <windowsx.h>
#include <commctrl.h>
#include <shlwapi.h>

#include <sore_fileio.h>
#include <sore_logger.h>

struct HDIR_MONITOR
{
    OVERLAPPED ol;
    HANDLE hDir;
    BYTE buffer[32 * 1024];
    LPARAM lParam;
    BOOL fStop;
    SORE_FileIO::file_callback callback;
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

bool RefreshMonitoring(HDIR_MONITOR* pMonitor);

static void CALLBACK MonitorCallback(DWORD dwErrorCode, 
                                     DWORD dwNumberOfBytesTransferred, 
                                     LPOVERLAPPED lpOverlapped)
{
    HDIR_MONITOR* pMonitor = (HDIR_MONITOR*)lpOverlapped;
    DWORD offset = 0;
    PFILE_NOTIFY_INFORMATION info;
    TCHAR szFile[MAX_PATH];

    if(dwErrorCode == ERROR_SUCCESS)
    {
        do
        {
            info = (PFILE_NOTIFY_INFORMATION) &pMonitor->buffer[offset];
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

            std::string filename(reinterpret_cast<char*>(szFile));
            filename = GetFullName(filename);

            ENGINE_LOG(SORE_Logging::LVL_INFO, std::string("change on ") + filename);

        } while(info->NextEntryOffset != 0);
    }
    if(!pMonitor->fStop)
    {
        RefreshMonitoring(pMonitor);
    }
}

static bool RefreshMonitoring(HDIR_MONITOR* pMonitor)
{
    // disable warning about forcing bool value to true/false
#pragma warning(push)
#pragma warning(disable: 4800)
    return ReadDirectoryChangesW(pMonitor->hDir, pMonitor->buffer, 32*1024, false, 
        FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
        0, &pMonitor->ol, MonitorCallback);
#pragma warning(pop)
}

void StopMonitoring(HDIR_MONITOR* pMonitor)
{
    if(pMonitor)
    {
        pMonitor->fStop = true;

        CancelIo(pMonitor->hDir);

        if(!HasOverlappedIoCompleted(&pMonitor->ol))
        {
            SleepEx(5, TRUE);
        }

        CloseHandle(pMonitor->ol.hEvent);
        CloseHandle(pMonitor->hDir);
        delete pMonitor;
    }
}

HDIR_MONITOR* StartMonitoring(const char* directory, SORE_FileIO::file_callback callback)
{
    HDIR_MONITOR* pMonitor = new HDIR_MONITOR;
    memset(pMonitor, 0, sizeof(HDIR_MONITOR));

    pMonitor->hDir = CreateFile(directory, 
        FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

    if (pMonitor->hDir != INVALID_HANDLE_VALUE)
    {
        pMonitor->ol.hEvent    = CreateEvent(NULL, TRUE, FALSE, NULL);
        pMonitor->callback     = callback;

        if (RefreshMonitoring(pMonitor))
        {
			return pMonitor;
		}
		else
		{
			CloseHandle(pMonitor->ol.hEvent);
			CloseHandle(pMonitor->hDir);
		}
	}

    delete pMonitor;
	return NULL;
}


SORE_FileIO::WindowsFileWatcher::WindowsFileWatcher() : finished(false)
{
}

static void CloseWatchHandle(std::pair<std::string, HANDLE> p)
{
    if(CloseHandle(p.second) == 0)
        ENGINE_LOG(SORE_Logging::LVL_WARNING, "Failed to close file handle");
}

SORE_FileIO::WindowsFileWatcher::~WindowsFileWatcher()
{
    finished = true;
}

void SORE_FileIO::WindowsFileWatcher::Pause()
{
}

void SORE_FileIO::WindowsFileWatcher::Resume()
{
}

void SORE_FileIO::WindowsFileWatcher::Frame(int elapsedTime)
{
    if(finished) return;

    MSG msg = {0};
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            finished = true;
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

SORE_FileIO::notify_handle SORE_FileIO::WindowsFileWatcher::Notify(
    const std::string& filename, file_callback callback)
{
    /*if(watches.find(GetParent(filename)) == watches.end())
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

        StartMonitoring(TEXT(GetParent(filename).c_str()), callback);

        return notify_handle();
    }
    else
    {
        return notify_handle();
    }*/
    return notify_handle();
}

void SORE_FileIO::WindowsFileWatcher::Remove(notify_handle handle)
{
    /*std::string filename = GetParent(handle.filename);
    watchedFiles[filename].callbacks.erase(handle.it);
    if(watchedFiles[filename].callbacks.empty())
        watchedFiles.erase(watchedFiles.find(filename));*/
}

#pragma warning( pop)