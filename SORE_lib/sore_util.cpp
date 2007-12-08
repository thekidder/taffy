
#include "sore_util.h"
#include "sore_logger.h"

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

float SORE_Utility::getRandomMinMax( float fMin, float fMax )
{
	float fRandNum = (float)rand () / RAND_MAX;
	return fMin + (fMax - fMin) * fRandNum;
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
		ENGINE_LOG_S(SORE_Logging::LVL_ERROR, "No file extension");
		return -1;
	}
	if(len-i>10)
	{
		ENGINE_LOG_S(SORE_Logging::LVL_ERROR, "Extension is too long");
		return -1;
	}
	strcpy(ext, filename+i+1);
	for(i=0;i<strlen(ext);i++)
	{
		ext[i] = tolower(ext[i]);
	}
	return 0;
}
