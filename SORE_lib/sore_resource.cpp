// $Id$

#include "sore_resource.h"
#include "sore_util.h"
#include "sore_fileio.h"

namespace SORE_Resource
{
	ResourceManager* Resource::rm = NULL;
	
	Resource::Resource(std::string file)
	{
		filename = file;
		SORE_FileIO::Notify(filename, std::bind1st(boost::mem_fn(&Resource::OnNotify), this) );
	}
	
	void Resource::AddDependentFile(std::string file)
	{
		dependentFiles.push_back(file);
		SORE_FileIO::Notify(file, std::bind1st(boost::mem_fn(&Resource::OnNotify), this) );
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
}
