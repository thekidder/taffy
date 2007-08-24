
#ifndef  __SORE_TEXTURE_H__
#define  __SORE_TEXTURE_H__

#include "sore_resource.h"

namespace SORE_Resource
{
	class Texture : public ResourceHandle
	{
		public:
			Texture(int iflags, const char* file) : ResourceHandle(iflags, file) {}
			void Load();
			void Load(const char* bytes, int len) {}
			void Unload() {}
			
			void LoadTGA(const char* filename);
			//void LoadTGA(const char* bytes, int len);
			
	};
	
	Texture* LoadTexture(const char* filename, int flags);
	//Texture* LoadDataTexture(const char* bytes, int len, int flags);
}

#endif /*__SORE_TEXTURE_H__*/
