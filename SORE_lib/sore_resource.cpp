
#include "sore_resource.h"
#include "sore_util.h"
#include <cstring>

SORE_Kernel::Resource::Resource()
{
	data = new char[1];
	len = 1;
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

SORE_Kernel::ResourceManager* SORE_Kernel::ResourceManager::rm = NULL;

SORE_Kernel::ResourceManager::ResourceManager()
{
	std::cout << "Resource handler created.\n";
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
	load_funcs[ext](filename);
}

SORE_Kernel::res_handle SORE_Kernel::ResourceManager::Register(char* bytes, int len, char* ext, int flags)
{
	std::map<char*, RES_LOAD>::iterator it = load_funcs.find(ext);
	if(it==load_funcs.end())
	{
		std::cerr << "no suitable loader available for ext " << ext << "\n";
		return -1;
	}
	load_data_funcs[ext](bytes,len);
}
		
void SORE_Kernel::ResourceManager::Unregister(res_handle resource)
{
}
		
SORE_Kernel::Resource* SORE_Kernel::ResourceManager::GetPtr(res_handle res)
{
}
		
SORE_Kernel::Resource* SORE_Kernel::ResourceManager::GetPtr(char* filename)
{
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
}
