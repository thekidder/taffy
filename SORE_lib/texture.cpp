
#include "texture.h"
#include "sore_util.h"
#include "fileio.h"

#include <fstream>
#include <cstring>
#include <string>
#include <cctype>
/*GLuint SORE_Texture::LoadBMP(const char* imgName)
{
	SDL_Surface* imgBuffer;
	GLuint tmpTexture;
	if( (imgBuffer = SDL_LoadBMP(imgName)) )
	{
		glGenTextures(1, &tmpTexture);
		glBindTexture(GL_TEXTURE_2D, tmpTexture);
		
		glTexImage2D( GL_TEXTURE_2D, 0, 3, imgBuffer->w,
			imgBuffer->h, 0, GL_BGR,
			GL_UNSIGNED_BYTE, imgBuffer->pixels );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		
		//std::cout << "t: " << tmpTexture << std::endl;
		//std::cout << "hmmm...\n";
		
		SDL_FreeSurface(imgBuffer);
		
		return tmpTexture;
	}
	else
	{
		SDL_FreeSurface(imgBuffer);
		return 0;
	}
}*/

GLuint SORE_Texture::LoadTGA(const char* imgName)
{
	bool failed = false;
	GLubyte header[18];
	GLuint texture;
	
	/*std::ifstream inFile;
	inFile.open(imgName, std::ifstream::binary);
	inFile.read((char*)header, 18);
	if(inFile.gcount()!=18) 
	{
		return 0; //failed
	}*/
	/*FILE* file = fopen(imgName, "rb");
	fread(header, 1, 18, file);*/
	
	SORE_FileIO::file_ref file = SORE_FileIO::Open(imgName);
	if(SORE_FileIO::Read(header, 1, 18, file)<18)
	{
		std::cerr << "Could not read header...corrupted file?\n";
		SORE_FileIO::Close(file);
		return 0;
	}
	
	std::cout << "Loaded header\nImage information:\n";
	std::cout << "Name: " << imgName << "\n";
	std::cout << "Width: " << int(header[12]+header[13]*256) << " Height: " << int(header[14]+header[15]*256) << "\n";
	std::cout << "BPP: " << (int)header[16] << " Image type: " << (int)header[2] << "\n";
	
	//do some basic checks to make sure we can handle the file
	
	GLubyte requested[18] = {0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	GLubyte mask     [18] = {0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0};
	
	for(int i=0;i<18;i++)
	{
		if(mask[i]!=0 && requested[i]!=header[i]) 
		{
			failed = true;
		}
	}
	
	if(failed) 
	{
		SORE_FileIO::Close(file);
		return 0;
	}
	
	int width, height, bpp;
	
	bpp    = int(header[16]);
	width  = int(header[12])+((int)header[13])*256;
	height = int(header[14])+((int)header[15])*256;
	
	char* filler = new char[int(header[0])];
	
	//inFile.read(filler, int(header[0]));
	if(SORE_FileIO::Read(filler, sizeof(char), int(header[0]), file)!=int(header[0]))
	{
		std::cerr << "Could not read filler...corrupted file?\n";
		SORE_FileIO::Close(file);
		return 0;
	}
	
	delete[] filler;
	
	unsigned int dataSize = width*height*(bpp/8);
	
	GLubyte* imgData = new GLubyte[dataSize];
	GLubyte temp;
	
	//inFile.read((char*)imgData, dataSize);
	if(SORE_FileIO::Read(imgData, 1, dataSize, file)!=dataSize)
	{
		delete[] imgData;
		std::cerr << "Could not read image data...corrupted file?\n";
		SORE_FileIO::Close(file);
		return 0;
	}
	
	for(int i=0;i<dataSize;i+=(bpp/8))
	{
		temp = imgData[i];
		imgData[i] = imgData[i+2];
		imgData[i+2] = temp;
	}
	
	//if(inFile.gcount()!=dataSize) return 0;
	
	//now for the actual opengl texture generation
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	GLint type;
	
	switch(bpp)
	{
		case 24:
			type = GL_RGB;
			break;
		case 32:
			type = GL_RGBA;
			break;
		default:
			delete[] imgData;
			return 0;
	}
	
	glTexImage2D( GL_TEXTURE_2D, 0, type, width,
				  height, 0, type,
	  GL_UNSIGNED_BYTE, imgData);

	/* Linear Filtering */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	delete[] imgData;
	//inFile.close();
	SORE_FileIO::Close(file);
	return texture;
}

int SORE_Texture::ImageFormat(const char* imgName, bool useNameMatching)
{
	if(useNameMatching)
	{
		char ext[10];
		SORE_Utility::GetFileExt(imgName, ext);
		std::cout << "ext: " << ext << std::endl;
		if(strcmp(ext, "tga")==0)
			return IMG_TGA;
		if(strcmp(ext, "bmp")==0)
			return IMG_BMP;
		return IMG_INVALID;
	}
	else
	{
		//not implemented
		return IMG_INVALID;
	}	
}
