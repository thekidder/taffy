// $Id$

#include "sore_resource.h"
#include "sore_util.h"
#include <cstring>
#include <iostream>

SORE_Resource::Resource::Resource(int iflags, const char* file)
{
	fromFile = true;
	flags = iflags;
	if(strlen(file)<255)
		strcpy(filename, file);
	else 
	{
		filename[0]='\0';
		std::cerr<<"filename too long\n";
		return;
	}
	if(flags & LOAD_IMMEDIATE)
	{
		//Load();
		loaded = true;
	}
	else
	{
		loaded = false;
	}
}

SORE_Resource::Resource::Resource(int iflags, const char* bytes, int len)
{
	fromFile = false;
	flags = iflags;
	if(!(flags & LOAD_IMMEDIATE))
	{
		std::cerr <<"[ERROR] this structure is in an inconsistent state\n";
		return;
	}
	loaded = true;
}

const char* SORE_Resource::Resource::GetFilename() const 
{
	return filename;
}

int SORE_Resource::Resource::GetFlags() const 
{
	return flags;
}

bool SORE_Resource::Resource::FromFile() const
{
	return fromFile;
}

SORE_Resource::ResourceData::ResourceData(int iflags, const char* file) : Resource(iflags, file)
{
	data = new char[1];
	length = 1;
}

SORE_Resource::ResourceData::ResourceData(int iflags, const char* bytes, int len) : Resource(iflags, bytes, len)
{
	data = new char[len];
	memcpy(data, bytes, len);
	length = len;
}

SORE_Resource::ResourceData::ResourceData(const ResourceData& r) : Resource(r.flags, r.filename)
{
	data = new char[r.length];
	length = r.length;
	memcpy(data, r.data, r.length);
}

SORE_Resource::ResourceData& SORE_Resource::ResourceData::operator=(const ResourceData& r)
{
	delete[] data;
	if(&r!=this)
	{
		data = new char[r.length];
		length = r.length;
		memcpy(data, r.data, r.length);
	}
	return *this;
}

SORE_Resource::ResourceData::~ResourceData()
{
	std::cout << "Freeing " << length << " bytes of data\n";
	delete[] data;
}

int SORE_Resource::ResourceData::GetLength() const 
{
	return length;
}

char* SORE_Resource::ResourceData::GetDataPtr() const 
{
	return data;
}

unsigned int SORE_Resource::ResourceHandle::GetHandle() const
{
	return handle;
}

SORE_Resource::ResourceManager* SORE_Resource::ResourceManager::rm = NULL;

SORE_Resource::ResourceManager::ResourceManager()
{
	//std::cout << "Resource manager created.\n";
	resources[0] = NULL;
}

void SORE_Resource::ResourceManager::Cleanup()
{
	std::map<res_handle, Resource*>::iterator temp,it=resources.begin();
	for(it++;it!=resources.end();)
	{
		temp = it;
		it++;
		delete temp->second;
		resources.erase(temp);
	}
	resources.clear();
}

SORE_Resource::ResourceManager* SORE_Resource::ResourceManager::GetManager()
{
	if(rm == NULL)
	{
		rm = new ResourceManager;
	}
	return rm;
}

SORE_Resource::res_handle SORE_Resource::ResourceManager::Register(const char* filename, int flags)
{
	if(filename[0]!='\0')
	{
		std::map<res_handle, Resource*>::iterator it;
		it=resources.begin();
		for(it++;it!=resources.end();it++)
		{
			if(it->second->FromFile() && strcmp(it->second->GetFilename(), filename)==0)
			{
				return it->first;
			}
		}
	}
	char ext[10];
	if(SORE_Utility::GetFileExt(filename, ext)!=0)
	{
		std::cerr << "could not determine extension\n";
		return -1;
	}
	std::map<const char*, RES_LOAD, equalstr>::iterator it = load_funcs.find(ext);
	if(it==load_funcs.end())
	{
		std::cerr << "no suitable loader available for ext " << ext << "\n";
		for(it=load_funcs.begin();it!=load_funcs.end();it++)
		{
			std::cout << "loader: " << it->first << " " << it->second << "\n";
		}
		return 0;
	}
	res_handle name = GetNextName();
	resources[name] = load_funcs[ext](filename,flags);
	return name;
}

SORE_Resource::res_handle SORE_Resource::ResourceManager::Register(const char* bytes, int len, const char* ext, int flags)
{
	std::map<const char*, RES_LOAD_DATA, equalstr>::iterator it = load_data_funcs.find(ext);
	if(it==load_data_funcs.end())
	{
		std::cerr << "no suitable loader available for ext " << ext << "\n";
		return 0;
	}
	res_handle name = GetNextName();
	resources[name] = load_data_funcs[ext](bytes,len,flags);
	return name;
}
		
void SORE_Resource::ResourceManager::Unregister(res_handle resource)
{
	int flags = resources[resource]->GetFlags();
	if(!(flags & CACHE_ALWAYS))
	{
		resources.erase(resource);
		names.push_front(resource);
	}
}

void SORE_Resource::ResourceManager::Unregister(const char* filename)
{
	std::map<res_handle, Resource*>::iterator it;
	it=resources.begin();
	for(it++;it!=resources.end();it++)
	{
		if(strcmp(it->second->GetFilename(), filename)==0)
		{
			int flags = it->second->GetFlags();
			if(!(flags & CACHE_ALWAYS))
			{
				delete it->second;
				resources.erase(it);
			}
		}
	}
}
		
SORE_Resource::Resource* SORE_Resource::ResourceManager::GetPtr(res_handle res)
{
	if(resources.find(res)==resources.end())
		return NULL;
	return resources[res];
}
		
SORE_Resource::Resource* SORE_Resource::ResourceManager::GetPtr(const char* filename)
{
	std::map<res_handle, Resource*>::iterator it;
	it=resources.begin();
	for(it++;it!=resources.end();it++)
	{
		if(strcmp(it->second->GetFilename(), filename)==0)
			return it->second;
	}
	return NULL;
}

void SORE_Resource::ResourceManager::RegisterLoader(RES_LOAD loader, const char* fileext)
{
	load_funcs[fileext] = loader;
}

void SORE_Resource::ResourceManager::RegisterDataLoader(RES_LOAD_DATA loader, const char* fileext)
{
	load_data_funcs[fileext] = loader;
}

void SORE_Resource::ResourceManager::Session()
{
	int flags;
	std::map<res_handle, Resource*>::iterator it,temp;
	it=resources.begin();
	for(it++;it!=resources.end();)
	{
		temp = it;
		it++;
		flags = it->second->GetFlags();
		if(flags & CACHE_SESSION)
		{
			delete temp->second;
			resources.erase(temp);
		}
	}
}

SORE_Resource::res_handle SORE_Resource::ResourceManager::GetNextName()
{
	if(names.empty())
	{
		return resources.size();
	}
	else
	{
		res_handle temp = names.back();
		names.pop_back();
		return temp;
	}
}

bool SORE_Resource::equalstr::operator()(const char* s1, const char* s2) const
{
	return strcmp(s1, s2) < 0;
}
