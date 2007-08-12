
#include "sore_resource.h"
#include "sore_util.h"
#include <cstring>

SORE_Kernel::Resource::Resource(int iflags)
{
	data = new char[1];
	len = 1;
	flags = iflags;
}

SORE_Kernel::Resource::Resource(const Resource& r)
{
	data = new char[r.len];
	len = r.len;
	memcpy(data, r.data, r.len);
}

SORE_Kernel::Resource& SORE_Kernel::Resource::operator=(const Resource& r)
{
	delete[] data;
	if(&r!=this)
	{
		data = new char[r.len];
		len = r.len;
		memcpy(data, r.data, r.len);
	}
	return *this;
}

SORE_Kernel::Resource::~Resource()
{
	delete[] data;
}

char* SORE_Kernel::Resource::GetDataPtr() const 
{
	return data;
}

const char* SORE_Kernel::Resource::GetFilename() const 
{
	return filename;
}

int SORE_Kernel::Resource::GetLength() const 
{
	return len;
}

int SORE_Kernel::Resource::GetFlags() const 
{
	return flags;
}

SORE_Kernel::ResourceManager* SORE_Kernel::ResourceManager::rm = NULL;

SORE_Kernel::ResourceManager::ResourceManager()
{
	std::cout << "Resource manager created.\n";
}

void SORE_Kernel::ResourceManager::Cleanup()
{
	for(std::map<res_handle, Resource*>::iterator it=resources.begin();it!=resources.end();it++)
	{
		delete it->second;
	}
	resources.clear();
}

/*static SORE_Kernel::ResourceManager* SORE_Kernel::ResourceManager::GetManager()
{
	if(rm == NULL)
	{
		rm = new ResourceManager;
	}
	return rm;
}*/

SORE_Kernel::res_handle SORE_Kernel::ResourceManager::Register(char* filename, int flags)
{
	char ext[10];
	if(SORE_Utility::GetFileExt(filename, ext)!=0)
	{
		std::cerr << "could not determine extension\n";
		return -1;
	}
	std::map<char*, RES_LOAD>::iterator it = load_funcs.find(ext);
	if(it==load_funcs.end())
	{
		std::cerr << "no suitable loader available for ext " << ext << "\n";
		return -1;
	}
	resources[resources.size()] = load_funcs[ext](filename,flags);
	return resources.size() - 1;
}

SORE_Kernel::res_handle SORE_Kernel::ResourceManager::Register(char* bytes, int len, char* ext, int flags)
{
	std::map<char*, RES_LOAD_DATA>::iterator it = load_data_funcs.find(ext);
	if(it==load_data_funcs.end())
	{
		std::cerr << "no suitable loader available for ext " << ext << "\n";
		return -1;
	}
	resources[resources.size()] = load_data_funcs[ext](bytes,len,flags);
	return resources.size() - 1;
}
		
void SORE_Kernel::ResourceManager::Unregister(res_handle resource)
{
	int flags = resources[resource]->GetFlags();
	if(!(flags & CACHE_ALWAYS))
	{
		resources[resource]->Unload();
		resources.erase(resource);
	}
}

void SORE_Kernel::ResourceManager::Unregister(char* filename)
{
	std::map<res_handle, Resource*>::iterator it;
	for(it=resources.begin();it!=resources.end();it++)
	{
		if(strcmp(it->second->GetFilename(), filename)==0)
		{
			int flags = it->second->GetFlags();
			if(!(flags & CACHE_ALWAYS))
			{
				it->second->Unload();
				resources.erase(it->first);
			}
		}
	}
}
		
SORE_Kernel::Resource* SORE_Kernel::ResourceManager::GetPtr(res_handle res)
{
	if(resources.find(res)==resources.end())
		return NULL;
	return resources[res];
}
		
SORE_Kernel::Resource* SORE_Kernel::ResourceManager::GetPtr(char* filename)
{
	std::map<res_handle, Resource*>::iterator it;
	for(it=resources.begin();it!=resources.end();it++)
	{
		if(strcmp(it->second->GetFilename(), filename)==0)
			return it->second;
	}
	return NULL;
}

void SORE_Kernel::ResourceManager::RegisterLoader(RES_LOAD loader, char* fileext)
{
	load_funcs[fileext] = loader;
}

void SORE_Kernel::ResourceManager::RegisterDataLoader(RES_LOAD_DATA loader, char* fileext)
{
	load_data_funcs[fileext] = loader;
}

void SORE_Kernel::ResourceManager::Session()
{
	int flags;
	std::map<res_handle, Resource*>::iterator it,temp;
	for(it=resources.begin();it!=resources.end();)
	{
		temp = it;
		it++;
		flags = it->second->GetFlags();
		if(flags & CACHE_SESSION)
		{
			it->second->Unload();
			resources.erase(temp);
		}
	}
}
