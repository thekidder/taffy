//SORE Texture and image subsystem

#include "allgl.h"
#include "fileio.h"

namespace SORE_Texture
{
	const int IMG_INVALID = 0;
	const int IMG_BMP     = 1;
	const int IMG_TGA     = 2;
	
	
	//Image loading functions
	//GLuint LoadBMP(const char* imgName); This relies upon SDL's bitmap load, which doesn't use SORE_FileIO. Commented out until a rewrite
	GLuint LoadTGA(const char* imgName);
	
	//Texture operation functions
	int ImageFormat(const char* imgName, bool useNameMatching = true); //return type of image. if useNameMatching is true, will determine based on extension. else, tries to read file to find out
	
}
