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
	class Buffer : public Resource
	{
		public:
			Buffer(int iflags) : Resource(iflags) {std::cout <<"Buffer instantiated\n";}
			~Buffer() {std::cout <<"Buffer killed\n";}
			void Load(char* filename);
			void Load(char* bytes, int len);
			void Unload();
			//int  GetLength() {return len;}
	};

	Buffer* LoadBuffer(char* filename, int flags);
	Buffer* LoadDataBuffer(char* bytes, int len, int flags);
}
