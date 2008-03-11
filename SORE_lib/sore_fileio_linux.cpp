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
#include <boost/shared_ptr.hpp>
#include "inotify-cxx.h"

namespace SORE_FileIO
{
	std::multimap<std::string, file_callback> callbacks;
	std::vector<boost::shared_ptr<InotifyWatch> > watches;
	static Inotify in;
	
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
				InotifyEvent ie;
				try
				{
					if(callbacks.empty()) return;
					
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
				catch(InotifyException e)
				{
					ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Caught Inotify exception: %s") % e.GetMessage());
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
		if(callbacks.find(filename)==callbacks.end())
		{
			boost::shared_ptr<InotifyWatch> iw(new InotifyWatch(filename, IN_MODIFY));
			watches.push_back(iw);
			//iw = new InotifyWatch(filename, IN_MODIFY);
			InotifyWatch* ptr = iw.get();
			try
			{
				in.Add(ptr);
				ENGINE_LOG(SORE_Logging::LVL_DEBUG1, boost::format("Added Inotify watch on path \"%s\"") % filename);
			}
			catch(InotifyException e)
			{
				ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Caught Inotify exception: %s") % e.GetMessage());
			}
		}
		callbacks.insert(std::pair<std::string, file_callback >(filename, callback));
	}
}
