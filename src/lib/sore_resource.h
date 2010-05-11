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
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
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
#include <boost/utility.hpp>

#include <sore_input.h>
#include <sore_fileio.h>
#include <sore_logger.h>
#include <sore_noncopyable.h>

namespace SORE_Resource
{
    class SORE_EXPORT WatchedFileArray;
    class SORE_EXPORT Resource;
    class SORE_EXPORT ResourcePool;

    typedef boost::shared_ptr<Resource> ResourcePtr;
    typedef boost::shared_ptr<WatchedFileArray> WatchedFileArrayPtr;
}

class SORE_EXPORT SORE_Resource::WatchedFileArray : SORE_Utility::Noncopyable
{
public:
    WatchedFileArray(const std::string& filename,
                     SORE_FileIO::PackageCache* pc = NULL,
                     SORE_FileIO::FilesystemNotifier* fn = NULL);
    WatchedFileArray(SORE_FileIO::PackageCache* pc = NULL,
                     SORE_FileIO::FilesystemNotifier* fn = NULL);
    ~WatchedFileArray();

    //Create new file and return it. The caller has the responsibility
    //to delete the file when finished
    SORE_FileIO::InFile* File(const std::string& name = "");
    std::string GetFilename() const;

    void SetNotifyFunction(SORE_FileIO::file_callback notifyFunction);
private:
    struct WatchInfo
    {
        std::string realPath;
        SORE_FileIO::notify_handle watch;
    };
    void InternalNotify(const std::string& file);
    void AddFile(const std::string& file);
    void RemoveWatches();

    std::map<std::string, WatchInfo> files;
    std::string defaultFile;

    SORE_FileIO::file_callback callback;

    SORE_FileIO::PackageCache* fileCache;
    SORE_FileIO::FilesystemNotifier* notifier;
};

class SORE_EXPORT SORE_Resource::Resource
{
public:
    Resource(WatchedFileArrayPtr wfa);
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

    WatchedFileArrayPtr watchedFiles;
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
    WatchedFileArrayPtr wfa(new WatchedFileArray(file, packageCache, notifier));
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
