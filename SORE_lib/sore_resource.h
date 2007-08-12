
#ifndef  __SORE_RESOURCE_H__
#define  __SORE_RESOURCE_H__

#include <map>

namespace SORE_Kernel
{
	//flags
	//caching - bits 1 and 2
	const int CACHE_ONDEMAND = 0x00;
	const int CACHE_SESSION  = 0x01;
	const int CACHE_ALWAYS   = 0x02;
		
	//load time - bit 3

	const int LOAD_LAZY      = 0x00;
	const int LOAD_IMMEDIATE = 0x04;
	
	class Resource
	{
		public:
			Resource(int iflags);
			~Resource();
			Resource& operator=(const Resource& r);
			Resource(const Resource& r);
			
			virtual void Load(char* file)=0;
			virtual void Load(char* bytes, int len)=0;
			virtual void Unload()=0;
			
			char* GetDataPtr() const;
			const char* GetFilename() const;
			int   GetLength() const;
			int   GetFlags() const;
		protected:
			char* data;
			int len;
			char filename[255];
			int flags;
	};
	
	typedef int res_handle;
	typedef Resource*(*RES_LOAD)(char*, int);
	typedef Resource*(*RES_LOAD_DATA)(char*,int, int);
	
	class ResourceManager
	{
		//Singleton members
		protected:
			ResourceManager();
			static ResourceManager* rm;
			
		public:
			static ResourceManager* GetManager() {if(rm==NULL){rm = new ResourceManager;}return rm;}
			
		//resouce manager members
		public:
			//registering/unregistering resources
			res_handle Register(char* filename, int flags = 0);
			res_handle Register(char* bytes, int len, char* ext, int flags = 0);
			void Unregister(res_handle resource);
			void Unregister(char* filename);

			//Fetch actual resource
			Resource* GetPtr(res_handle res);
			Resource* GetPtr(char* filename);
			
			//caching control
			void Session();
			
			//register resource loaders
			void RegisterLoader(RES_LOAD loader, char* fileext);
			void RegisterDataLoader(RES_LOAD_DATA loader, char* fileext);
			
			//cleanup
			void Cleanup();
		protected:
			std::map<res_handle, Resource*> resources;
			std::map<char*, RES_LOAD> load_funcs;
			std::map<char*, RES_LOAD_DATA> load_data_funcs;
	};
}

#endif /*__SORE_RESOURCE_H__*/
