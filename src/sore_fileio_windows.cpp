/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 // $Id$

#pragma warning(disable : 4995)

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <psapi.h>
#include <strsafe.h>

#include "sore_fileio.h"
#include "sore_logger.h"

namespace SORE_FileIO
{
#define BUFSIZE 512

/*BOOL GetFileNameFromHandle(HANDLE hFile) 
{
  BOOL bSuccess = FALSE;
  TCHAR pszFilename[MAX_PATH+1];
  HANDLE hFileMap;

  // Get the file size.
  DWORD dwFileSizeHi = 0;
  DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi); 

  if( dwFileSizeLo == 0 && dwFileSizeHi == 0 )
  {
     printf("Cannot map a file with a length of zero.\n");
     return FALSE;
  }

  // Create a file mapping object.
  hFileMap = CreateFileMapping(hFile, 
                    NULL, 
                    PAGE_READONLY,
                    0, 
                    MAX_PATH,
                    NULL);

  if (hFileMap) 
  {
    // Create a file mapping to get the file name.
    void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

    if (pMem) 
    {
      if (GetMappedFileName (GetCurrentProcess(), 
                             pMem, 
                             pszFilename,
                             MAX_PATH)) 
      {

        // Translate path with device name to drive letters.
        TCHAR szTemp[BUFSIZE];
        szTemp[0] = '\0';

        if (GetLogicalDriveStrings(BUFSIZE-1, szTemp)) 
        {
          TCHAR szName[MAX_PATH];
          TCHAR szDrive[3] = TEXT(" :");
          BOOL bFound = FALSE;
          TCHAR* p = szTemp;

          do 
          {
            // Copy the drive letter to the template string
            *szDrive = *p;

            // Look up each device name
            if (QueryDosDevice(szDrive, szName, BUFSIZE))
            {
              UINT uNameLen = _tcslen(szName);

              if (uNameLen < MAX_PATH) 
              {
                bFound = _tcsnicmp(pszFilename, szName, 
                    uNameLen) == 0;

                if (bFound) 
                {
                  // Reconstruct pszFilename using szTempFile
                  // Replace device path with DOS path
                  TCHAR szTempFile[MAX_PATH];
                  StringCchPrintf(szTempFile,
                            MAX_PATH,
                            TEXT("%s%s"),
                            szDrive,
                            pszFilename+uNameLen);
                  StringCchCopyN(pszFilename, MAX_PATH+1, szTempFile, _tcslen(szTempFile));
                }
              }
            }

            // Go to the next NULL character.
            while (*p++);
          } while (!bFound && *p); // end of string
        }
      }
      bSuccess = TRUE;
      UnmapViewOfFile(pMem);
    } 

    CloseHandle(hFileMap);
  }
  _tprintf(TEXT("File name is %s\n"), pszFilename);
  return(bSuccess);
}

*/	
	std::multimap<std::string, file_callback> callbacks;
	
	HANDLE hDir;

	DWORD WINAPI ThreadFunc(LPVOID args)
	{
		HANDLE file;
		file = FindFirstChangeNotification(".\\", FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
		WaitForSingleObject(file, INFINITE);
		while(true)
		{
			FindNextChangeNotification(file);
			WaitForSingleObject(file, INFINITE);
			HANDLE i = file;
		}
		//GetFileNameFromHandle(file);
		return 0;
	}

	class WindowsFileWatch : public SORE_Kernel::Task
	{
		public:
			WindowsFileWatch(SORE_Kernel::GameKernel* gk)
			{
				ENGINE_LOG(SORE_Logging::LVL_INFO, "Spawning file watch thread");
				/*thread = CreateThread(NULL,
					0,
					ThreadFunc,
					NULL,
					0,
					&dwThreadId);*/
				file = FindFirstChangeNotification(".\\", FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
				WaitForSingleObject(file, 0);
			}
			~WindowsFileWatch()
			{
				
			}
			
			void Pause() {}
			void Resume() {}
			void Frame(int elapsedTime) 
			{
				/*FindNextChangeNotification(file);
				WaitForSingleObject(file, 0);
				if(file!=INVALID_HANDLE_VALUE)
				{
					ENGINE_LOG(SORE_Logging::LVL_INFO, "change");
				}*/
			}
			
			const char* GetName() const { return "File Notify task";}
		protected:
			HANDLE thread, file;	
			DWORD dwThreadId;
	};
	
	bool InitFileNotify(SORE_Kernel::GameKernel* gk)
	{
		static WindowsFileWatch watchTask(gk);
		gk->AddConstTask(0, 5000, &watchTask);
		return true;
	}
	
	void Notify(std::string filename, boost::function<void (std::string filename)> callback)
	{
		//watches.push_back(InotifyWatch(filename, IN_MODIFY));
		//in.Add(*(watches.end()-1));
		callbacks.insert(std::pair<std::string, file_callback >(filename, callback));
	}
}
