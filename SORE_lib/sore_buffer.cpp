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

void SORE_Resource::Buffer::Load(char* filename)
{
}

void SORE_Resource::Buffer::Load(char* bytes, int len)
{
	std::cout << "creating empty buffer of " << len << " bytes.\n";
	delete[] data;
	data = new char[len];
}

void SORE_Resource::Buffer::Unload()
{
	std::cout <<"Buffer unloaded\n";
}

SORE_Resource::Buffer* SORE_Resource::LoadBuffer(char* filename, int flags)
{
	Buffer* b = new Buffer(flags);
	b->Load(filename);
	return b;
}

SORE_Resource::Buffer* SORE_Resource::LoadDataBuffer(char* bytes, int len, int flags)
{
	Buffer* b = new Buffer(flags);
	b->Load(bytes, len);
	return b;
}
