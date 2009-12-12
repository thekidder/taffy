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

#include <list>
#include <map>

#include <boost/utility.hpp>

#include "3rdparty/inotify-cxx.h"

namespace SORE_FileIO
{
    class LinuxInotifyWatcher : public FilesystemNotifier, boost::noncopyable
    {
    public:
        LinuxInotifyWatcher();
        ~LinuxInotifyWatcher();

        void Pause();
        void Resume();
        void Frame(int elapsedTime);

        const char* GetName() const { return "Linux Inotify watcher task"; }

        virtual notify_handle Notify(
            const std::string& filename, file_callback callback);
        virtual void Remove(notify_handle handle);
    private:
        void AddWatch(const std::string& filename, int32_t mask);
        void AddParentWatch(const std::string& filename);
        void RemoveWatch(InotifyWatch* iw);

        bool ParentWatchExists(const std::string& filename);
        void PerformCallback(const std::string& path);

        struct watch_info
        {
            std::list<std::pair<std::string, file_callback> > callbacks;
        };
        std::map<std::string, watch_info> watchedFiles;
        std::vector<InotifyWatch*> watches;

        Inotify in;
    };

#define FilesystemWatcher LinuxInotifyWatcher
}
