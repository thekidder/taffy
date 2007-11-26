
#include "sore_texture.h"
#include "sore_util.h"
#include "sore_fileio.h"
#include "sore_allgl.h"
#include "sore_logger.h"

void SORE_Resource::Texture::Load()
{
	char ext[10];
	SORE_Utility::GetFileExt(filename, ext);
	if(strcmp(ext, "tga")==0)
	{
		LoadTGA(filename);
	}
	else
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, "Unknown file format %s", ext);
	}
}

void SORE_Resource::Texture::LoadTGA(const char* filename)
{
	handle = 0;
	bool failed = false;
	GLubyte header[18];
	GLuint texture;
	
	SORE_FileIO::file_ref file = SORE_FileIO::Open(filename);
	if(file == 0 ) 
	{
		ENGINE_LOG_S(SORE_Logging::LVL_ERROR, "Could not open texture file");
		return;
	}
	if(SORE_FileIO::Read(header, 1, 18, file)<18)
	{
		ENGINE_LOG_S(SORE_Logging::LVL_ERROR, "Could not read header...corrupted file?");
		SORE_FileIO::Close(file);
		return;
	}
	
	ENGINE_LOG_S(SORE_Logging::LVL_DEBUG2, "Loaded header");
	ENGINE_LOG(SORE_Logging::LVL_DEBUG2, "Name: %s", filename);
	ENGINE_LOG(SORE_Logging::LVL_DEBUG2, "Width: %d Height: %d", int(header[12]+header[13]*256), int(header[14]+header[15]*256));
	ENGINE_LOG(SORE_Logging::LVL_DEBUG2, "BPP: %d Image type: %d", (int)header[16], (int)header[2]);
	
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
		return;
	}
	
	int width, height, bpp;
	
	bpp    = int(header[16]);
	width  = int(header[12])+((int)header[13])*256;
	height = int(header[14])+((int)header[15])*256;
	
	char* filler = new char[int(header[0])];
	
	if(SORE_FileIO::Read(filler, sizeof(char), int(header[0]), file)!=int(header[0]))
	{
		ENGINE_LOG_S(SORE_Logging::LVL_ERROR, "Could not read filler...corrupted file?");
		SORE_FileIO::Close(file);
		return;
	}
	
	delete[] filler;
	
	unsigned int dataSize = width*height*(bpp/8);
	
	GLubyte* imgData = new GLubyte[dataSize];
	GLubyte temp;
	
	if(SORE_FileIO::Read(imgData, 1, dataSize, file)!=dataSize)
	{
		delete[] imgData;
		ENGINE_LOG_S(SORE_Logging::LVL_ERROR, "Could not read image data...corrupted file?");
		SORE_FileIO::Close(file);
		return;
	}
	
	for(int i=0;i<dataSize;i+=(bpp/8))
	{
		temp = imgData[i];
		imgData[i] = imgData[i+2];
		imgData[i+2] = temp;
	}
	
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
			return;
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
	handle = texture;
}

SORE_Resource::Texture* SORE_Resource::LoadTexture(const char* filename, int flags)
{
	Texture* t = new Texture(flags, filename);
	t->Load();
	return t;
}

/*SORE_Resource::Texture* SORE_Resource::LoadDataTexture(const char* bytes, int len, int flags)
{
	Texture* t = new Texture(flags);
	t->Load(bytes, len);
	return t;
}*/
