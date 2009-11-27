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
#include <vector>
#include <string>

#include <boost/function.hpp>
#include <boost/functional/hash.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "sore_input.h"
#include "sore_fileio.h"
#include "sore_logger.h"

namespace SORE_Resource
{
    class SORE_EXPORT Resource;

    typedef boost::shared_ptr<Resource> ResourcePtr;

    class SORE_EXPORT ResourcePool;
}

/*
  As a little bit of a hack, we will put the logic for reloading textures
  into ResourcePool.
  To do this, we add a new field to Resource: GLContextDependent, which
  returns true if we need to reload on GL context change
*/

class SORE_EXPORT SORE_Resource::Resource
{
public:
    Resource(std::string file, SORE_FileIO::PackageCache* pc = NULL,
             std::string info = "", bool delayedNotify = false);
    Resource(SORE_FileIO::PackageCache* pc = NULL);
    virtual ~Resource();
    virtual const char* Type() const {return "generic resource";}

    std::string GetFilename() const {return filename;}

    static void SetRM(ResourcePool* _rm) {rm = _rm;}

    virtual void Reload() {Load();}
    virtual bool GLContextDependent() const {return false;}
protected:
    virtual void Load() = 0;
    void OnNotify(std::string file);

    //used for file notification - for example, a shader is made up of
    //multiple files, we want to reload it if the shader files are changed
    void AddDependentFile(std::string file);
    void SetFilename(std::string file);
    bool IsDependent(std::string file);
    std::string additionalInfo;
    SORE_FileIO::PackageCache* packageCache;
    std::vector<std::string> dependentFiles;
    static ResourcePool* rm;
private:
    void SetupNotify(std::string filename);

    std::string filename;
};

class SORE_EXPORT SORE_Resource::ResourcePool
{
public:
    ResourcePool(SORE_FileIO::PackageCache* pc = NULL);

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
    ResourcePtr r(new T(filename, packageCache));
    boost::weak_ptr<Resource> temp = r;
    resources.insert(std::make_pair(hash, temp));
    boost::shared_ptr<T> resource = boost::dynamic_pointer_cast<T>(r);
    return resource;
}


#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
