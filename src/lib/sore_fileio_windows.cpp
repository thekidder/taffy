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

#include <sore_defines.h> //for windows.h
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <psapi.h>
#include <strsafe.h>

#include <sore_fileio.h>
#include <sore_logger.h>

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

#pragma warning( pop )