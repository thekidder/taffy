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

#include <algorithm>

#include "sore_logger.h"
#include "sore_fileio.h"

SORE_FileIO::LinuxInotifyWatcher::LinuxInotifyWatcher()
{
    in.SetNonBlock(true);
    ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Inotify: max events: %d")
               % Inotify::GetCapability(IN_MAX_EVENTS));
    ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Inotify: max instances: %d")
               % Inotify::GetCapability(IN_MAX_INSTANCES));
    ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Inotify: max watches: %d")
               % Inotify::GetCapability(IN_MAX_WATCHES));
}

SORE_FileIO::LinuxInotifyWatcher::~LinuxInotifyWatcher()
{
    std::vector<InotifyWatch*>::iterator it;
    for(it = watches.begin(); it != watches.end(); ++it)
    {
        in.Remove(*it);
        delete *it;
    }
}

void SORE_FileIO::LinuxInotifyWatcher::Pause()
{
}

void SORE_FileIO::LinuxInotifyWatcher::Resume()
{
}

static std::string GetParent(const std::string& filename)
{
    if(filename.rfind("/") == std::string::npos)
        return std::string(".");
    std::string parent = filename.substr(0, filename.rfind("/"));
    if(parent.empty())
        parent = std::string("/");
    return parent;
}

void SORE_FileIO::LinuxInotifyWatcher::PerformCallback(const std::string& path)
{
    std::string parent = GetParent(path);
    std::map<std::string, watch_info>::iterator it;
    if((it = watchedFiles.find(parent)) != watchedFiles.end())
    {
        std::list<std::pair<std::string, file_callback> >::iterator jt;
        for(jt = it->second.callbacks.begin();
            jt != it->second.callbacks.end(); ++jt)
        {
            if(jt->first == path)
            {
                SORE_FileIO::file_callback c = jt->second;
                c(path);
            }
        }
    }
}

void SORE_FileIO::LinuxInotifyWatcher::Frame(int elapsedTime)
{
    InotifyEvent ie;
    try
    {
        if(watchedFiles.empty()) return;

        in.WaitForEvents();
        if(in.GetEventCount()==0) return;
        in.GetEvent(ie);
        std::string filename = ie.GetWatch()->GetPath() + "/" + ie.GetName();
        std::string types;
        ie.DumpTypes(types);
        ENGINE_LOG(SORE_Logging::LVL_DEBUG2,
                   boost::format("%s received on %s")
                   % types % filename);

        if(!ie.IsType(IN_DELETE_SELF) && !ie.IsType(IN_IGNORED))
        {
            PerformCallback(filename);
        }
        else if(ie.IsType(IN_IGNORED))
        {
            RemoveWatch(ie.GetWatch());
        }
    }
    catch(InotifyException e)
    {
        ENGINE_LOG(
            SORE_Logging::LVL_ERROR,
            boost::format("Caught Inotify exception: %s") % e.GetMessage());
    }
}

void SORE_FileIO::LinuxInotifyWatcher::AddWatch(const std::string& filename,
                                                int32_t mask)
{
    InotifyWatch* iw = new InotifyWatch(filename, mask);
    watches.push_back(iw);
    try
    {
        in.Add(iw);
        ENGINE_LOG(
            SORE_Logging::LVL_DEBUG1,
            boost::format("Added Inotify watch on path \"%s\"") % filename);
    }
    catch(InotifyException e)
    {
        ENGINE_LOG(
            SORE_Logging::LVL_ERROR,
            boost::format("Caught Inotify exception: %s") % e.GetMessage());
        ENGINE_LOG(
            SORE_Logging::LVL_ERROR,
            boost::format("    Watch path: %s") % iw->GetPath());
    }

}

void SORE_FileIO::LinuxInotifyWatcher::AddParentWatch(const std::string& filename)
{
    if(!ParentWatchExists(filename))
        AddWatch(GetParent(filename), IN_CREATE | IN_MODIFY);
}

void SORE_FileIO::LinuxInotifyWatcher::RemoveWatch(InotifyWatch* iw)
{
    std::vector<InotifyWatch*>::iterator it;
    it = std::find(watches.begin(), watches.end(), iw);
    if(it != watches.end())
    {
        ENGINE_LOG(SORE_Logging::LVL_DEBUG1,
                   "Removed watch on " + iw->GetPath());
        in.Remove(iw);
        delete iw;
        watches.erase(it);
    }
}

SORE_FileIO::notify_handle SORE_FileIO::LinuxInotifyWatcher::Notify(
    const std::string& filename, file_callback callback)
{
    std::string file = filename;
    if(file.find("/") == std::string::npos)
    {
        file = "./" + file;
    }
    if(watchedFiles.find(file)==watchedFiles.end())
    {
        AddParentWatch(file);
        watchedFiles.insert(std::make_pair(file, watch_info()));
    }
    watchedFiles[GetParent(file)].callbacks.push_front(std::make_pair(file, callback));
    return notify_handle(file, watchedFiles[GetParent(file)].callbacks.begin());
}

bool SORE_FileIO::LinuxInotifyWatcher::ParentWatchExists(const std::string& filename)
{
    std::string parent = GetParent(filename);
    std::map<std::string, watch_info>::iterator it;
    for(it = watchedFiles.begin(); it != watchedFiles.end(); ++it)
    {
        std::string thisParent = GetParent(it->first);
        if(thisParent == parent)
            return true;
    }
    return false;
}

void SORE_FileIO::LinuxInotifyWatcher::Remove(notify_handle handle)
{
    std::string filename = GetParent(handle.filename);
    watchedFiles[filename].callbacks.erase(handle.it);
    if(watchedFiles[filename].callbacks.empty())
        watchedFiles.erase(watchedFiles.find(filename));
}
