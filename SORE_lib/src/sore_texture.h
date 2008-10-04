// $Id$

#ifndef  __SORE_TEXTURE_H__
#define  __SORE_TEXTURE_H__

#include "sore_resource.h"
#include "sore_allgl.h"

namespace SORE_Resource
{
	class Texture2D : public Resource
	{
		public:
			Texture2D(std::string filename, std::string additionalInfo);
			GLuint GetHandle() const;
			void Bind() const;
			const char* Type() const {return "2D texture";}
			
		protected:
			void Load();
			void LoadTGA(const char* filename);
			void Unload() {}
			GLuint handle;
	};
}

#endif /*__SORE_TEXTURE_H__*/
