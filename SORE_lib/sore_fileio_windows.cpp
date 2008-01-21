/***************************************************************************
 *   Copyright (C) 2007 by Adam Kidder   *
 *   thekidder@gmail.com   *
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

#include "sore_fileio.h"
#include <windows.h>

namespace SORE_FileIO
{
	std::multimap<std::string, file_callback> callbacks;
	
	HANDLE hDir;

	DWORD WINAPI ThreadFunc(LPVOID args)
	{
		//std::cout << SORE_Logging::LVL_INFO, "ThreadFunc called";
		hDir = CreateFile(".\\",
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ|FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL);
		if(hDir==INVALID_HANDLE_VALUE)
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Failed to get directory handle");
		FILE_NOTIFY_INFORMATION buffer[1024];
		DWORD bytesReturned;
		while(ReadDirectoryChangesW(hDir,
			&buffer,
			sizeof(buffer),
			TRUE,
			FILE_NOTIFY_CHANGE_LAST_WRITE,
			&bytesReturned,
			NULL,
			NULL))
		{
			ENGINE_LOG(SORE_Logging::LVL_INFO, "directory modified");
		}
		int err = GetLastError();
		return 0;
	}

	class WindowsFileWatch : public SORE_Kernel::Task
	{
		public:
			WindowsFileWatch(SORE_Kernel::GameKernel* gk) : Task(gk)
			{
				ENGINE_LOG(SORE_Logging::LVL_INFO, "Spawning file watch thread");
				thread = CreateThread(NULL,
					0,
					ThreadFunc,
					NULL,
					0,
					&dwThreadId);
			}
			~WindowsFileWatch()
			{
				
			}
			
			void Pause() {}
			void Resume() {}
			void Frame(int elapsedTime) 
			{
			}
			
			const char* GetName() const { return "File Notify task";}
		protected:
			HANDLE thread;	
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
