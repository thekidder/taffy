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

namespace SORE_FileIO
{
	std::multimap<std::string, file_callback> callbacks;
	
	class WindowsFileWatch : public SORE_Kernel::Task
	{
		public:
			WindowsFileWatch(SORE_Kernel::GameKernel* gk) : Task(gk) {}
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
			
	};
	
	bool InitFileNotify(SORE_Kernel::GameKernel* gk)
	{
		return true;
	}
	
	void Notify(std::string filename, boost::function<void (std::string filename)> callback)
	{
		//watches.push_back(InotifyWatch(filename, IN_MODIFY));
		//in.Add(*(watches.end()-1));
		callbacks.insert(std::pair<std::string, file_callback >(filename, callback));
	}
}
