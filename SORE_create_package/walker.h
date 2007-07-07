
#include <vector>

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

void Walk(const char* directory, unsigned short int top, file_list& files);
