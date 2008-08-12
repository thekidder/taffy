// $Id$

#ifndef  __SORE_RESOURCE_H__
#define  __SORE_RESOURCE_H__

#include <map>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/functional/hash.hpp>
#include "sore_logger.h"

namespace SORE_Resource
{
	class ResourcePool;
	
	class Resource
	{
		public:
			Resource(std::string file, std::string info="");
			Resource() {filename="Not loaded from file";}
			virtual ~Resource();
			virtual const char* Type() const {return "generic resource";}
			
			std::string GetFile() const {return filename;}
			
			static void SetRM(ResourcePool* _rm) {rm = _rm;}
			
			virtual void Reload() {Load();}
		protected:
			virtual void Load() = 0;
			void OnNotify(std::string file);
			void AddDependentFile(std::string file); //used for file notification - for example, a shader is made up of multiple files, we want to reload it if the shader files are changed
			bool IsDependent(std::string file);
			std::string filename;
			std::string additionalInfo;
			std::vector<std::string> dependentFiles;
			static ResourcePool* rm;
	};
	
	class ResourcePool
	{
		public:
			ResourcePool();
			~ResourcePool();
			
			template<class T>
					T* GetResource(std::string filename, std::string additionalInfo="")
			{
				std::size_t hash = string_hash(filename+additionalInfo);
				if(resources.find(hash)==resources.end())
				{
					T* temp = new T(filename, additionalInfo);
					resources.insert(std::pair<std::size_t, Resource*>(hash, temp) );
					return temp;
				}
				else
				{
					//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Retrieved resource: %s") % filename);
					Resource* r = resources.find(hash)->second;
					T* resource = dynamic_cast<T*>(r);
					if(resource==NULL) ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Could not downcast resource for filename %s") % filename);
					return resource;
				}
			}
			
			void for_each(boost::function<void (Resource*)> func);
		protected:
			std::map<std::size_t, Resource*> resources;
		private:
			boost::hash<std::string> string_hash;
	};
}

#endif /*__SORE_RESOURCE_H__*/
