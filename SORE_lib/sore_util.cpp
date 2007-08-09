
#include "sore_util.h"

bool CheckExtension(const char* extensionName)
{
	char* extensionsList = (char*) glGetString(GL_EXTENSIONS);
	
	while(*extensionsList)
	{
		size_t length = strcspn(extensionsList, " ");
		
		if(length == strlen(extensionName) && strstr(extensionsList, extensionName)==0)
		{
			return true;
		}
		extensionsList += length+1;
	}
	return false;
}

float getRandomMinMax( float fMin, float fMax )
{
	float fRandNum = (float)rand () / RAND_MAX;
	return fMin + (fMax - fMin) * fRandNum;
}

float Distance(float x1, float y1, float z1, float x2, float y2, float z2)
{
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
}

int SORE_Utility::GetFileExt(const char* filename, char* ext)
{
	int len = strlen(filename);
	int i;
	for(i=len-1;i>=0;i--)
	{
		if(filename[i]=='.')
			break;
	}
	if(i==0) 
	{
		std::cerr << "no ext\n";
		return -1;
	}
	if(len-i>10)
	{
		std::cerr << "too long ext\n";
		return -1;
	}
	strcpy(ext, filename+i+1);
	for(i=0;i<strlen(ext);i++)
	{
		ext[i] = tolower(ext[i]);
	}
	return 0;
}
