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

#include <sore_resource.h>
#include <sore_util.h>
#include <sore_fileio.h>

#include <functional>
#include <algorithm>
#include <boost/bind.hpp>

SORE_Resource::WatchedFileArray::WatchedFileArray(
    const std::string& filename,
    SORE_FileIO::PackageCache* pc,
    SORE_FileIO::FilesystemNotifier* fn)
    : fileCache(pc), notifier(fn)
{
    AddFile(filename);
}

SORE_Resource::WatchedFileArray::WatchedFileArray(
    SORE_FileIO::PackageCache* pc,
    SORE_FileIO::FilesystemNotifier* fn)
    : fileCache(pc), notifier(fn)
{
}

SORE_Resource::WatchedFileArray::~WatchedFileArray()
{
    RemoveWatches();
}

SORE_FileIO::InFile* SORE_Resource::WatchedFileArray::File(const std::string& name)
{
    std::string file;
    if(name.empty())
        file = defaultFile;
    else
        file = name;
    std::map<std::string, WatchInfo>::iterator it = files.find(file);
    if(it == files.end())
        AddFile(file);
    it = files.find(file);
    SORE_FileIO::InFile* f = new SORE_FileIO::InFile(it->first.c_str(), fileCache);
    return f;
}

std::string SORE_Resource::WatchedFileArray::GetFilename() const
{
    std::map<std::string, WatchInfo>::const_iterator it = files.find(defaultFile);
    if(it == files.end())
        return std::string("");
    return it->first;
}

void SORE_Resource::WatchedFileArray::SetNotifyFunction(
    SORE_FileIO::file_callback notifyFunction)
{
    callback = notifyFunction;
}

void SORE_Resource::WatchedFileArray::AddFile(const std::string& file)
{
    std::string realFile = file;
    if(fileCache && fileCache->Contains(file.c_str()))
        realFile = fileCache->PackageFilename(file.c_str());
    SORE_FileIO::notify_handle hnd;
    if(notifier)
        hnd = notifier->Notify(
            realFile, boost::bind(&WatchedFileArray::InternalNotify, this, _1));
    WatchInfo wi = {realFile, hnd};
    files.insert(std::make_pair(file, wi));
    if(defaultFile.empty())
        defaultFile = file;
}

void SORE_Resource::WatchedFileArray::InternalNotify(const std::string& file)
{
    if(callback)
        callback(file);
}

void SORE_Resource::WatchedFileArray::RemoveWatches()
{
    if(notifier)
    {
        std::map<std::string, WatchInfo>::iterator it;
        for(it = files.begin(); it != files.end(); ++it)
        {
            notifier->Remove(it->second.watch);
        }
    }
}

SORE_Resource::Resource::Resource(WatchedFileArrayPtr wfa)
    : pool(0), watchedFiles(wfa)
{
    if(watchedFiles)
        watchedFiles->SetNotifyFunction(boost::bind(&Resource::OnNotify, this, _1));
}

SORE_Resource::Resource::~Resource()
{
}

std::string SORE_Resource::Resource::GetFilename() const
{
    if(watchedFiles)
        return watchedFiles->GetFilename();
    else
        return std::string("Not loaded from file");
}

SORE_FileIO::InFile* SORE_Resource::Resource::File(const std::string& name)
{
    assert(watchedFiles);
    return watchedFiles->File(name);
}

void SORE_Resource::Resource::SetResourcePool(ResourcePool* rp)
{
    pool = rp;
}

void SORE_Resource::Resource::OnNotify(const std::string& file)
{
    ENGINE_LOG(SORE_Logging::LVL_INFO,
               boost::format("Reloading resource file %s") % file);
    Load();
}

SORE_Resource::ResourcePool::ResourcePool(SORE_FileIO::PackageCache* pc,
                                          SORE_FileIO::FilesystemNotifier* fn)
  : packageCache(pc), notifier(fn)
{
}

void SORE_Resource::ResourcePool::for_each(boost::function<void (ResourcePtr)> func)
{
    std::map<std::size_t, boost::weak_ptr<Resource> >::iterator it;
    for(it=resources.begin();it!=resources.end();it++)
    {
        if(ResourcePtr r = it->second.lock())
        {
            func(r);
        }
    }
}

bool SORE_Resource::ResourcePool::OnResize(SORE_Kernel::Event* e)
{
    ENGINE_LOG(SORE_Logging::LVL_DEBUG1, "Reloading all GL dependent resources");
    std::map<std::size_t, boost::weak_ptr<Resource> >::iterator it;
    for(it=resources.begin();it!=resources.end();it++)
    {
        if(ResourcePtr r = it->second.lock())
        {
            if(r->GLContextDependent())
                r->Reload();
        }
    }
    return true;
}

