
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