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
        return std::string("./");
    std::string parent = filename.substr(0, filename.rfind("/"));
    if(parent.empty())
        parent = std::string("/");
    return parent;
}

void SORE_FileIO::LinuxInotifyWatcher::PerformCallback(const std::string& path)
{
    std::map<std::string, watch_info>::iterator it;
    if((it = watchedFiles.find(path)) != watchedFiles.end())
    {
        std::list<file_callback>::iterator jt;
        for(jt = it->second.callbacks.begin();
            jt != it->second.callbacks.end(); ++jt)
        {
            SORE_FileIO::file_callback c = *jt;
            c(path);
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
        std::string types;
        std::string path = ie.GetWatch()->GetPath();
        ie.DumpTypes(types);
        ENGINE_LOG(SORE_Logging::LVL_DEBUG2,
                   boost::format("Event path name: %s; event %s")
                   % path % types);

        if(ie.IsType(IN_CREATE))
        {
            std::string filename = path + "/" + ie.GetName();
            AddWatch(filename);
            PerformCallback(filename);
        }
        else if(ie.IsType(IN_DELETE_SELF))
        {
        }
        else if(ie.IsType(IN_IGNORED))
        {
            RemoveWatch(ie.GetWatch());
        }
        else
        {
            PerformCallback(path);
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
        AddWatch(GetParent(filename), IN_CREATE);
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
    if(watchedFiles.find(file)==watchedFiles.end())
    {
        AddWatch(filename);
        AddParentWatch(filename);
        watchedFiles.insert(std::make_pair(filename, watch_info()));
    }
    watchedFiles[filename].callbacks.push_front(callback);
    return notify_handle(filename, watchedFiles[filename].callbacks.begin());
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
    watchedFiles[handle.filename].callbacks.erase(handle.it);
}
