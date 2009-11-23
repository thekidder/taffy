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

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/functional/hash.hpp>

#include "sore_input.h"
#include "sore_fileio.h"
#include "sore_logger.h"

class SORE_EXPORT SORE_Resource::ResourcePool;

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
    ~ResourcePool();

    template<typename T>
        T* GetResource(std::string filename)
    {
        std::size_t hash = string_hash(filename);
        if(resources.find(hash)==resources.end())
        {
            T* temp = new T(filename, packageCache);
            resources.insert(std::pair<std::size_t, Resource*>(hash, temp) );
            return temp;
        }
        else
        {
            Resource* r = resources.find(hash)->second;
            T* resource = dynamic_cast<T*>(r);
            if(resource==NULL)
                ENGINE_LOG(SORE_Logging::LVL_ERROR,
                           boost::format("Could not downcast resource for filename %s")
                           % filename);
            return resource;
        }
    }
    //searchs for r in resource map, then unloads it. returns true if it was unloaded,
    //false if not found
    bool UnloadResource(Resource* r);

    void for_each(boost::function<void (Resource*)> func);
    bool OnResize(SORE_Kernel::Event* e);
protected:
    std::map<std::size_t, Resource*> resources;
private:
    boost::hash<std::string> string_hash;
    SORE_FileIO::PackageCache* packageCache;
};

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
