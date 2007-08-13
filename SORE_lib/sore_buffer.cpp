//
// C++ Implementation: sore_buffer
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "sore_buffer.h"

void SORE_Resource::Buffer::Load(const char* filename)
{
}

void SORE_Resource::Buffer::Load(const char* bytes, int len)
{
	
	delete[] data;
	data = new char[len];
	if(bytes[0]=='\0')
		std::cout << "creating empty buffer of " << len << " bytes.\n";
	else
	{
		std::cout << "creating (non-empty) buffer of " << len << " bytes.\n";
		memcpy(data, bytes, len);
	}
}

void SORE_Resource::Buffer::Unload()
{
	std::cout <<"Buffer unloaded\n";
}

SORE_Resource::Buffer* SORE_Resource::LoadBuffer(const char* filename, int flags)
{
	Buffer* b = new Buffer(flags, filename);
	b->Load(filename);
	return b;
}

SORE_Resource::Buffer* SORE_Resource::LoadDataBuffer(const char* bytes, int len, int flags)
{
	Buffer* b = new Buffer(flags);
	b->Load(bytes, len);
	return b;
}
