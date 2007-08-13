//
// C++ Interface: sore_buffer
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "sore_resource.h"
#include <iostream>

namespace SORE_Resource
{
	class Buffer : public ResourceData
	{
		public:
			Buffer(int iflags) : ResourceData(iflags) {std::cout <<"Buffer instantiated\n";}
			Buffer(int iflags, const char* file) : ResourceData(iflags, file) {std::cout <<"Buffer instantiated\n";}
			~Buffer() {std::cout <<"Buffer killed\n";}
			void Load(const char* filename);
			void Load(const char* bytes, int len);
			void Unload();
			//int  GetLength() {return len;}
	};

	Buffer* LoadBuffer(const char* filename, int flags);
	Buffer* LoadDataBuffer(const char* bytes, int len, int flags);
}
