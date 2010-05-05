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
