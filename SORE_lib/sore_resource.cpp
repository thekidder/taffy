// $Id$

#include "sore_resource.h"
#include "sore_util.h"
#include "sore_fileio.h"

#include <functional>
#include <algorithm>
#include <boost/bind.hpp>

namespace SORE_Resource
{
	ResourceManager* Resource::rm = NULL;
	
	Resource::Resource(std::string file)
	{
		filename = file;
		boost::function<void (std::string)> callback = std::bind1st(std::mem_fun(&Resource::OnNotify),this);
		SORE_FileIO::Notify(filename, callback );
	}
	
	void Resource::AddDependentFile(std::string file)
	{
		dependentFiles.push_back(file);
		boost::function<void (std::string)> callback = std::bind1st(std::mem_fun(&Resource::OnNotify),this);
		SORE_FileIO::Notify(file, callback );
	}
	
	ResourceManager::ResourceManager()
	{
	}
	
	void Resource::OnNotify(std::string file)
	{
		ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Reloading resource file %s") % file);
		Load();
	}
	
	ResourceManager::~ResourceManager()
	{
		std::map<std::string, Resource*>::iterator it;
		
		for(it=resources.begin();it!=resources.end();it++)
		{
			delete it->second;
		}
	}
	
	void ResourceManager::for_each(boost::function<void (Resource*)> func)
	{
		//std::for_each(resources.begin(), resources.end(), boost::bind(func));
		for(std::map<std::string, Resource*>::iterator it=resources.begin();it!=resources.end();it++)
		{
			Resource* r = it->second;
			func(r);
		}
	}
}
