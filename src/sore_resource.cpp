/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// $Id$

#include "sore_resource.h"
#include "sore_util.h"
#include "sore_fileio.h"

#include <functional>
#include <algorithm>
#include <boost/bind.hpp>

namespace SORE_Resource
{
  ResourcePool* Resource::rm = NULL;
	
  Resource::Resource(std::string file, std::string info, bool delayedNotify)
		: additionalInfo(info)
  {
    if(!delayedNotify)
      SetFilename(file);
  }
  
  Resource::~Resource()
  {
  }

  void Resource::SetFilename(std::string file)
  {
    boost::function<void (std::string)> callback = 
			std::bind1st(std::mem_fun(&Resource::OnNotify),this);
    filename = file;
    SORE_FileIO::Notify(filename, callback);
  }
  
  void Resource::AddDependentFile(std::string file)
  {
    if(!IsDependent(file))
		{
			dependentFiles.push_back(file);
			boost::function<void (std::string)> callback = 
				std::bind1st(std::mem_fun(&Resource::OnNotify),this);
			SORE_FileIO::Notify(file, callback );
		}
  }
  
  bool Resource::IsDependent(std::string file)
  {
		std::vector<std::string>::iterator it;
    for(it=dependentFiles.begin();it!=dependentFiles.end();it++)
		{
			if(*it == file)
				return true;
		}
    return false;
  }
  
  void Resource::OnNotify(std::string file)
  {
    ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Reloading resource file %s") % file);
    Load();
  }
  
  ResourcePool::ResourcePool()
  {
  }
  
  ResourcePool::~ResourcePool()
  {
    std::map<std::size_t, Resource*>::iterator it;
		
    for(it=resources.begin();it!=resources.end();++it)
		{
			delete it->second;
		}
	}
	
	bool ResourcePool::UnloadResource(Resource* r)
	{
		std::map<std::size_t, Resource*>::iterator it;
		for(it=resources.begin();it!=resources.end();++it)
		{
			if(it->second == r)
			{
				delete r;
				resources.erase(it);
				return true;
			}
		}
		return false;
	}

	void ResourcePool::for_each(boost::function<void (Resource*)> func)
	{
		//std::for_each(resources.begin(), resources.end(), boost::bind(func));
		std::map<std::size_t, Resource*>::iterator it;
		for(it=resources.begin();it!=resources.end();it++)
		{
			Resource* r = it->second;
			func(r);
		}
	}

	bool ResourcePool::OnResize(SORE_Kernel::Event* e)
	{
		ENGINE_LOG(SORE_Logging::LVL_DEBUG1, "Reloading all GL dependent resources");
		std::map<std::size_t, Resource*>::iterator it;
		for(it=resources.begin();it!=resources.end();it++)
		{
			if(it->second->GLContextDependent())
				it->second->Reload();
		}
		return true;
	}
}
