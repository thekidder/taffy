
#include "walker.h"

#include <dirent.h> 
#include <sys/types.h> 
#include <sys/param.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <cstdio> 
#include <string.h> 

void Walk(const char* directory, unsigned short int top, file_list& files)
{
	DIR           *d;
	struct dirent *dir;
	file_info temp;
	d = opendir(directory);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			temp.fileID = files.size()+1;
			temp.parentID = top;
			temp.file = true;
			strcpy(temp.filename, dir->d_name);
			strcpy(temp.fullname, directory);
			strcat(temp.fullname, "/");
			strcat(temp.fullname, dir->d_name);
			if(!strcmp(dir->d_name, "."))
				continue;
			if(!strcmp(dir->d_name, ".."))
				continue;
			if(dir->d_type == DT_DIR)
			{
				temp.file = false;
			}
			
			files.push_back(temp);
			
			if(dir->d_type == DT_DIR)
			{
				char* dirname;
				dirname = new char[strlen(directory)+strlen(dir->d_name)+2];
				strcpy(dirname, directory);
				strcat(dirname, "/");
				strcat(dirname, dir->d_name);
				Walk(dirname, temp.fileID, files);
				delete[] dirname;
			}
			//printf("%s\n", dir->d_name);
		}

		closedir(d);
	}
}
