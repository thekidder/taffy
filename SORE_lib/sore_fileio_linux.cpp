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
#include <map>
#include "inotify-cxx.h"

namespace SORE_FileIO
{
	std::multimap<std::string, file_callback> callbacks;
	std::vector<InotifyWatch> watches;
	Inotify in;
	
	class LinuxINotifyWatch : public SORE_Kernel::Task
	{
		public:
			LinuxINotifyWatch(SORE_Kernel::GameKernel* gk) : Task(gk) {}
			~LinuxINotifyWatch()
			{
				std::multimap<std::string, file_callback>::iterator it;
				for(it=callbacks.begin();it!=callbacks.end();it++)
				{
					
				}
			}
			
			void Pause() {}
			void Resume() {}
			void Frame(int elapsedTime) 
			{
				if(callbacks.empty()) return;
				InotifyEvent ie;
				in.WaitForEvents();
				if(in.GetEventCount()==0) return;
				in.GetEvent(ie);
				std::string path = ie.GetWatch()->GetPath().empty() ?  "" : ie.GetWatch()->GetPath();
				ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Event path name: %s") % path);
				std::multimap<std::string, file_callback>::iterator it;
				it = callbacks.find(path);
				while(it!=callbacks.end() && it->first == path)
				{
					it->second(it->first);
					it++;
				}
			}
			
			const char* GetName() const { return "File Notify task";}
		protected:
			
	};
	
	bool InitFileNotify(SORE_Kernel::GameKernel* gk)
	{
		in.SetNonBlock(true);
		static LinuxINotifyWatch watchTask(gk);
		gk->AddConstTask(0, 5000, &watchTask);
		ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Inotify: max events: %d") % Inotify::GetCapability(IN_MAX_EVENTS));
		ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Inotify: max instances: %d") % Inotify::GetCapability(IN_MAX_INSTANCES));
		ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Inotify: max watchs: %d") % Inotify::GetCapability(IN_MAX_WATCHES));
		return true;
	}
	
	void Notify(std::string filename, boost::function<void (std::string)> callback)
	{
		watches.push_back(InotifyWatch(filename, IN_MODIFY));
		in.Add(*(watches.end()-1));
		/*if()
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Failed to start watch on file %s") % filename);
		}*/
		//else
		callbacks.insert(std::pair<std::string, file_callback >(filename, callback));
	}
}
