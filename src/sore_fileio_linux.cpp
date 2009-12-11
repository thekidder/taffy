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
}

void SORE_FileIO::LinuxInotifyWatcher::Pause()
{
}

void SORE_FileIO::LinuxInotifyWatcher::Resume()
{
}

void SORE_FileIO::LinuxInotifyWatcher::Frame(int elapsedTime)
{
    InotifyEvent ie;
    try
    {
        if(callbacks.empty()) return;

        in.WaitForEvents();
        if(in.GetEventCount()==0) return;
        in.GetEvent(ie);
        std::string types;
        std::string path = ie.GetWatch()->GetPath();
        ie.DumpTypes(types);
        ENGINE_LOG(SORE_Logging::LVL_DEBUG2,
                   boost::format("Event path name: %s; event %s")
                   % path % types);
        if(ie.IsType(IN_DELETE_SELF))
        {
            /*std::string path = ie.GetWatch()->GetPath();
            std::string filename = path.substr(0, path.rfind('/')+1);
            InotifyWatch* iw = new InotifyWatch(filename, IN_CREATE);
            watches.push_back(iw);
            in.Add(iw);
            ENGINE_LOG(SORE_Logging::LVL_DEBUG1,
            "Adding watch on " + filename);*/
            return;
        }
        if(ie.IsType(IN_IGNORED))
        {
            ENGINE_LOG(SORE_Logging::LVL_DEBUG1,
                       "Removed watch on " + ie.GetWatch()->GetPath());
            RemoveWatch(ie.GetWatch());
            return;
        }
        if(callbacks.find(path) != callbacks.end())
        {
            SORE_FileIO::file_callback c = callbacks.find(path)->second;
            c(path);
        }
    }
    catch(InotifyException e)
    {
        ENGINE_LOG(
            SORE_Logging::LVL_ERROR,
            boost::format("Caught Inotify exception: %s") % e.GetMessage());
    }
}

void SORE_FileIO::LinuxInotifyWatcher::RemoveWatch(InotifyWatch* iw)
{
    std::vector<InotifyWatch*>::iterator it;
    it = std::find(watches.begin(), watches.end(), iw);
    if(it != watches.end())
    {
        in.Remove(iw);
        delete iw;
        watches.erase(it);
    }
}

void SORE_FileIO::LinuxInotifyWatcher::Notify(
    const std::string& filename, file_callback callback)
{
    std::string file = filename;//.substr(0, filename.rfind("/"));
    if(file.empty())
        file = "./";
    if(callbacks.find(file)==callbacks.end())
    {
        InotifyWatch* iw = new InotifyWatch(
            file, IN_MODIFY | IN_MOVE | IN_MOVE_SELF | IN_DELETE_SELF
            | IN_CREATE | IN_DELETE);
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
                boost::format("Watch path: %s") % iw->GetPath());
        }
    }
    callbacks.insert(std::make_pair(file, callback));
}

void SORE_FileIO::LinuxInotifyWatcher::Remove(
    const std::string& filename, file_callback callback)
{
    /*std::map<std::string, file_callback>::iterator it;
    for(it = callbacks.begin(); it != callbacks.end(); ++it)
    {
        if(filename == it->first && callback == it->second)
        {
            callbacks.erase(it);
            break;
        }
    }
    */
}
