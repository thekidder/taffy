
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

Uint8* SORE_Utility::GetBMPMask(const char* filename)
{
	static Uint8* mask;
	Uint8 alpha;
	Uint32 pixel,temp;
	SDL_Surface* bmp = SDL_LoadBMP(filename);
	mask = new Uint8[bmp->w*bmp->h];
	SDL_PixelFormat* pf = bmp->format;
	SDL_LockSurface(bmp);
	
	for(int i=0;i<bmp->w*bmp->h;i++)
	{
		pixel=*((Uint32*)bmp->pixels+(Uint32)(i*pf->BytesPerPixel));
		temp=pixel&pf->Amask; /* Isolate alpha component */
		temp=temp>>pf->Ashift;/* Shift it down to 8-bit */
		temp=temp<<pf->Aloss; /* Expand to a full 8-bit number */
		alpha=(Uint8)temp;
		mask[i] = alpha;
	}
	SDL_UnlockSurface(bmp);
	return mask;
}