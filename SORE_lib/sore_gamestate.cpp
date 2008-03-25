/***************************************************************************
 *   Copyright (C) 2008 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// $Id$

#include "sore_gamestate.h"

namespace SORE_Kernel
{
	/*Buffer::Buffer(size_t len) : length(len)
	{
		buf = new char[len];
	}
	
	Buffer::Buffer(const Buffer& b) : length(b.length)
	{
		buf = new char[length];
		for(size_t i=0;i<length;i++)
		{
			buf[i] = b.buf[i];
		}
	}
	
	Buffer::~Buffer()
	{
		if(buf) delete[] buf;
	}
			
	char    Buffer::operator[](size_t pos)
	{
		assert(pos<length);
		return buf[pos];
	}
	
	Buffer& Buffer::operator=(const Buffer& b)
	{
		if(b!=*this)
		{
			if(buf!=NULL) delete[] buf;
			length = b.length;
			buf = new char[length];
			for(size_t i=0;i<length;i++)
			{
				buf[i] = b.buf[i];
			}
		}
		return *this;
	}
			
	char*   Buffer::head()
	{
		return buf;
	}
	
	size_t  Buffer::size()
	{
		return length;
	}
	
	void    Buffer::resize(size_t newLen)
	{
		size_t toCopy = newLen;
		if(toCopy>length) toCopy = length;
		char* temp = new char[newLen];
		for(size_t i=0;i<toCopy;i++)
		{
			temp[i] = buf[i];
		}
		delete[] buf;
		buf = new char[newLen];
		for(size_t i=0;i<toCopy;i++)
		{
			buf[i] = temp[i];
		}
		delete[] temp;
		length = newLen;
	}*/
}
