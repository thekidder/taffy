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

#include <iostream>
#include <cstdio>
#include <cstring>

#include <boost/filesystem.hpp>

#include "walker.h"

unsigned short int AddFile(boost::filesystem::path p, unsigned short int top, std::string prefix, file_list& files)
{
    file_info temp;
    temp.fileID = files.size()+1;
    temp.parentID = top;
    if(!boost::filesystem::exists(p))
        temp.file = false;
    else if(boost::filesystem::is_directory(boost::filesystem::status(p)))
        temp.file = false;
    else
        temp.file = true;
    strcpy(temp.filename, p.filename().c_str());
    strcpy(temp.fullname, p.string().c_str());

    std::cout << "Adding " << prefix << temp.fullname << " to archive\n";

    files.push_back(temp);

    return temp.fileID;
}

void Walk(boost::filesystem::path p, unsigned short int top, std::string prefix, file_list& files)
{
    boost::filesystem::directory_iterator end;
    if(!boost::filesystem::exists(p))
    {
        std::cerr << "Could not open " << p.string() << std::endl;
        return;
    }

    top = AddFile(p, top, prefix, files);

    for(boost::filesystem::directory_iterator it(p); it != end; ++it)
    {
        if(!boost::filesystem::is_directory(it->status()))
            AddFile(it->path(), top, prefix, files);
        else
        {
            char* dirname;
            dirname = new char[strlen(it->path().string().c_str())+2];
            strcpy(dirname, it->path().string().c_str());
            Walk(dirname, top, prefix, files);
            delete[] dirname;
        }
    }
}
