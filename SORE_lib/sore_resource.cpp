
#include "sore_resource.h"

ResourceManager* SORE_Kernel::ResourceManager::rm = NULL;

SORE_Kernel::ResourceManager::ResourceManager()
{
	std::cout << "Resource handler created.\n";
}

static ResourceManager* SORE_Kernel::ResourceManager::GetManager()
{
	if(rm == NULL)
	{
		rm = new ResourceManager();
	}
	return rm;
}

res_handle SORE_Kernel::ResourceManager::Register(char* filename, int flags = 0)
{
}

res_handle SORE_Kernel::ResourceManager::Register(char* bytes, int len, int type, int flags = 0)
{
}
		
void SORE_Kernel::ResourceManager::Unregister(res_handle resource)
{
}
		
Resource* SORE_Kernel::ResourceManager::GetPtr(res_handle res)
{
}
		
Resource* SORE_Kernel::ResourceManager::GetPtr(char* filename)
{
}
		
void SORE_Kernel::ResourceManager::Session()
{
}