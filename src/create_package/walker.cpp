/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#include <iostream>
#include <cstdio>
#include <cstring>

#include <boost/filesystem.hpp>

#include "walker.h"

// disable warnings about strcpy v strcpy_s
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

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

	strcpy(temp.filename, p.filename().string().c_str());
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

#ifdef _MSC_VER
#pragma warning(pop)
#endif