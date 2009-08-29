/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
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

#include <string>
#include <vector>

#include <boost/filesystem.hpp>

struct file_info
{
	unsigned short int fileID;
	unsigned short int parentID;
	bool file;
	
	char filename[256];
	char fullname[512];
	
	unsigned int pos;
	unsigned int size;
	unsigned int sizeRaw;
};

typedef std::vector<file_info> file_list;

unsigned short int AddFile(boost::filesystem::path p, unsigned short int parent, std::string prefix, file_list& files);
void Walk(boost::filesystem::path p, unsigned short int top, std::string prefix, file_list& files);
