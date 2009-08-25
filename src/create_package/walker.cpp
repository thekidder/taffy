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
            unsigned short int id = AddFile(it->path(), top, prefix, files);
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
