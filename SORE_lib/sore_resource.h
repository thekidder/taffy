
#ifndef  __SORE_RESOURCE_H__
#define  __SORE_RESOURCE_H__

namespace SORE_Kernel
{
	//flags
	//caching - bits 1 and 2
	int CACHE_ONDEMAND = 0x00;
	int CACHE_SESSION  = 0x01;
	int CACHE_ALWAYS   = 0x02;
		
	//load time - bit 3
	int LOAD_LAZY      = 0x00;
	int LOAD_IMMEDIATE = 0x04;
	
	class Resource
	{
	};
	
	typedef int res_handle;
	class ResourceManager
	{
		//Singleton members
		protected:
			ResourceManager();
			static ResourceManager* rm;
		public:
			static ResourceManager* GetManager();
			
		//resouce manager members
		public:
			//registering/unregistering resources
			res_handle Register(char* filename, int flags = 0);
			res_handle Register(char* bytes, int len, int type, int flags = 0);
			void Unregister(res_handle resource);
			
			//Fetch actual resource
			Resource* GetPtr(res_handle res);
			Resource* GetPtr(char* filename);
			
			//caching control
			void Session();
	};
}

#endif /*__SORE_RESOURCE_H__*/