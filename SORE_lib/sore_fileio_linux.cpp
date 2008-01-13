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

#include "sore_fileio.h"
#include <map>
#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>

namespace SORE_FileIO
{
	std::multimap<std::string, file_callback> callbacks;
	
	class LinuxINotifyWatch : public SORE_Kernel::Task
	{
		public:
			LinuxINotifyWatch(SORE_Kernel::GameKernel* gk) : Task(gk) {}
			~LinuxINotifyWatch()
			{
				std::multimap<std::string, file_callback>::iterator it;
				for(it=callbacks.begin();it!=callbacks.end();it++)
				{
					inotifytools_remove_watch_by_filename(it->first.c_str());
				}
			}
			
			void Pause() {}
			void Resume() {}
			void Frame(int elapsedTime) 
			{
				if(callbacks.empty()) return;
				inotify_event* event;
				do
				{
					event = inotifytools_next_event(0);
					if(event==NULL) break;
					char buffer[1024];
					inotifytools_snprintf(buffer, 1024, event, "%w");
					std::multimap<std::string, file_callback>::iterator it = callbacks.find(buffer);
					while(it!=callbacks.end() && it->first==buffer)
					{
						it->second(it->first);
						it++;
					}
				}
				while(event!=NULL);
			}
			
			const char* GetName() const { return "File Notify task";}
		protected:
			
	};
	
	bool InitFileNotify(SORE_Kernel::GameKernel* gk)
	{
		bool success =  (bool)inotifytools_initialize();
		if(success)
		{
			static LinuxINotifyWatch watchTask(gk);
			gk->AddConstTask(0, 1000, &watchTask);
		}
	}
	
	void Notify(std::string filename, boost::function<void (std::string filename)> callback)
	{
		if(inotifytools_watch_file(filename.c_str(), IN_MODIFY)==0)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Failed to start watch on file %s") % filename);
		}
		else
			callbacks.insert(std::pair<std::string, file_callback >(filename, callback));
	}
}
