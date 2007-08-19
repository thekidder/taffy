/*
  At the time, consider the resource management in all files currently BROKEN.
  It compiles, but there is no guarantee it works or doesn't crash or leak memory or behave badly in any other way.
  It will be fixed eventually, but needs an entire redesign. The 'design' it currently has is ugly, hard to maintain,
  hard to extend, and generally not froody.
*/



#ifndef  __SORE_RESOURCE_H__
#define  __SORE_RESOURCE_H__

#include <map>
#include <deque>

namespace SORE_Resource
{
	//c-string comparison for map
	struct equalstr
	{
		bool operator()(const char* s1, const char* s2) const;
	};
	
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
			Resource(int iflags, const char* file);
			Resource(int iflags, const char* bytes, int len);
			virtual ~Resource() {}
			
			virtual void Load()=0; //load from file (called from constructor or by ResourceHandler)
			
			const char* GetFilename() const;
			int   GetFlags() const;
			bool  FromFile() const;
			bool  IsLoaded() const {return loaded;}
		protected:
			
			bool fromFile, loaded;
			char filename[255];
			int flags;
	};
	
	class ResourceData : public Resource
	{
		public:
			ResourceData(int iflags, const char* bytes, int len);
			ResourceData(int iflags, const char* file);
			ResourceData& operator=(const ResourceData& r);
			ResourceData(const ResourceData& r);
			~ResourceData();
			
			char* GetDataPtr() const;
			int   GetLength() const;
		protected:
			char* data;
			int length;
	};
	
	class ResourceHandle : public Resource
	{
		public:
			ResourceHandle(int iflags, const char* bytes, int len) : Resource(iflags, bytes, len) {handle = 0;}
			ResourceHandle(int iflags, const char* file) : Resource(iflags, file) {handle = 0;}
			unsigned int GetHandle() const;
			
		protected: 
			unsigned int handle;
	};
	
	typedef unsigned int res_handle; //resource name
	typedef Resource*(*RES_LOAD)(const char*, int);
	typedef ResourceData*(*RES_LOAD_DATA)(const char*,int, int);
	
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
			res_handle Register(const char* filename, int flags = 0);
			res_handle Register(const char* bytes, int len, const char* ext, int flags = 0);
			void Unregister(res_handle resource);
			void Unregister(const char* filename);

			//Fetch actual resource
			Resource* GetPtr(res_handle res);
			Resource* GetPtr(const char* filename);
			
			//caching control
			void Session();
			
			//register resource loaders
			void RegisterLoader(RES_LOAD loader, const char* fileext);
			void RegisterDataLoader(RES_LOAD_DATA loader, const char* fileext);
			
			//cleanup
			void Cleanup();
		protected:
			std::map<res_handle, Resource*> resources;
			std::map<const char*, RES_LOAD, equalstr> load_funcs;
			std::map<const char*, RES_LOAD_DATA, equalstr> load_data_funcs;
		private:
			std::deque<res_handle> names; //holds freed names for resources
			res_handle GetNextName();
	};
}



#endif /*__SORE_RESOURCE_H__*/
