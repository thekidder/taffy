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

#ifndef  SORE_RESOURCE_H
#define  SORE_RESOURCE_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <map>
#include <string>
#include <vector>

#include <boost/function.hpp>
#include <boost/functional/hash.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "sore_input.h"
#include "sore_fileio.h"
#include "sore_logger.h"

namespace SORE_Resource
{
    class SORE_EXPORT WatchedFileArray;
    class SORE_EXPORT Resource;
    class SORE_EXPORT ResourcePool;

    typedef boost::shared_ptr<Resource> ResourcePtr;
}

class SORE_EXPORT SORE_Resource::WatchedFileArray
{
public:
    WatchedFileArray(const std::string& filename,
                     SORE_FileIO::PackageCache* pc = NULL,
                     SORE_FileIO::FilesystemNotifier* fn = NULL);
    WatchedFileArray(SORE_FileIO::PackageCache* pc = NULL,
                     SORE_FileIO::FilesystemNotifier* fn = NULL);

    //Create new file and return it. The caller has the responsibility
    //to delete the file when finished
    SORE_FileIO::InFile* File(const std::string& name = "");
    std::string GetFilename() const;

    void SetNotifyFunction(SORE_FileIO::file_callback notifyFunction);
private:
    //WatchedFileArray(const WatchedFileArray& wfa);
    //WatchedFileArray& operator=(const WatchedFileArray& wfa);

    void InternalNotify(const std::string& file);

    void AddFile(const std::string& file);

    //pair<filename, processed_filename>
    std::map<std::string, std::string> files;
    std::string defaultFile;

    SORE_FileIO::file_callback callback;

    SORE_FileIO::PackageCache* fileCache;
    SORE_FileIO::FilesystemNotifier* notifier;
};

class SORE_EXPORT SORE_Resource::Resource
{
public:
    Resource(const WatchedFileArray& wfa);
    virtual ~Resource();
    virtual const char* Type() const {return "generic resource";}

    std::string GetFilename() const;

    virtual void Reload() {Load();}
    virtual bool GLContextDependent() const {return false;}

    void SetResourcePool(ResourcePool* rp);
protected:
    virtual void Load() = 0;

    SORE_FileIO::InFile* File(const std::string& name = "");

    ResourcePool* pool;
private:
    void OnNotify(const std::string& file);

    WatchedFileArray watchedFiles;
};

/*
  As a little bit of a hack, we will put the logic for reloading textures
  into ResourcePool.
  To do this, we add a new field to Resource: GLContextDependent, which
  returns true if we need to reload on GL context change
*/
class SORE_EXPORT SORE_Resource::ResourcePool
{
public:
    ResourcePool(SORE_FileIO::PackageCache* pc = NULL,
                 SORE_FileIO::FilesystemNotifier* fn = NULL);

    template<typename T>
        boost::shared_ptr<T> GetResource(const std::string& filename);

    void for_each(boost::function<void (ResourcePtr)> func);
    bool OnResize(SORE_Kernel::Event* e);
private:
    template<typename T>
        boost::shared_ptr<T> InsertResource(
            const std::string& filename, std::size_t hash);

    boost::hash<std::string> string_hash;
    SORE_FileIO::PackageCache* packageCache;
    SORE_FileIO::FilesystemNotifier* notifier;

    std::map<std::size_t, boost::weak_ptr<Resource> > resources;
};

template<typename T>
boost::shared_ptr<T> SORE_Resource::ResourcePool::GetResource(
    const std::string& filename)
{
    std::size_t hash = string_hash(filename);
    if(resources.find(hash)==resources.end())
    {
        return InsertResource<T>(filename, hash);
    }
    else
    {
        boost::weak_ptr<Resource> r = resources.find(hash)->second;
        if(ResourcePtr temp = r.lock())
        {
            boost::shared_ptr<T> temp_casted = boost::dynamic_pointer_cast<T>(temp);
            return temp_casted;
        }
        else
        {
            return InsertResource<T>(filename, hash);
        }
    }
}

template<typename T>
boost::shared_ptr<T> SORE_Resource::ResourcePool::InsertResource(
    const std::string& filename, std::size_t hash)
{
    std::string file = T::ProcessFilename(filename);
    WatchedFileArray wfa(file, packageCache, notifier);
    ResourcePtr r(new T(wfa));
    r->SetResourcePool(this);
    boost::weak_ptr<Resource> temp = r;
    resources.insert(std::make_pair(hash, temp));
    boost::shared_ptr<T> resource = boost::dynamic_pointer_cast<T>(r);
    return resource;
}


#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
