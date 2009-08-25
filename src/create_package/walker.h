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
