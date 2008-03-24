// $Id$

#include "sore_texture.h"
#include "sore_util.h"
#include "sore_fileio.h"
#include "sore_allgl.h"
#include "sore_logger.h"

void SORE_Resource::Texture2D::Load()
{
	if(handle!=0)
		glDeleteTextures(1, &handle);
	char ext[10];
	SORE_Utility::GetFileExt(filename.c_str(), ext);
	if(strcmp(ext, "tga")==0)
	{
		LoadTGA(filename.c_str());
	}
	else
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Unknown file format %s") % ext);
	}
}

void SORE_Resource::Texture2D::LoadTGA(const char* filename)
{
	handle = 0;
	bool failed = false;
	GLubyte header[18];
	
	SORE_FileIO::file_ref file = SORE_FileIO::Open(filename);
	if(file == 0 ) 
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Could not open texture file '%s'") % filename);
		return;
	}
	if(SORE_FileIO::Read(header, 1, 18, file)<18)
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not read header...corrupted file?");
		SORE_FileIO::Close(file);
		return;
	}
	
	ENGINE_LOG(SORE_Logging::LVL_DEBUG2, "Loaded header");
	ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Name: %s") % filename);
	ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Width: %d Height: %d") % int(header[12]+header[13]*256) % int(header[14]+header[15]*256));
	ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("BPP: %d Image type: %d") % (int)header[16] % (int)header[2]);
	
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
		ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not read filler...corrupted file?");
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
		ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not read image data...corrupted file?");
		SORE_FileIO::Close(file);
		return;
	}
	
	for(size_t i=0;i<dataSize;i+=(bpp/8))
	{
		temp = imgData[i];
		imgData[i] = imgData[i+2];
		imgData[i+2] = temp;
	}
	
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	
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
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Unrecognized image type - only supports 24bit or 32bit (uncompressed) TGAs");
			delete[] imgData;
			return;
	}
	
	glTexImage2D( GL_TEXTURE_2D, 0, type, width,
								height, 0, type,
				GL_UNSIGNED_BYTE, imgData);

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	delete[] imgData;
	//inFile.close();
	SORE_FileIO::Close(file);
	
	GLenum error;
	while((error=glGetError())!=GL_NO_ERROR)
	{
		ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Texture: GL Error: %d") % error);
	}
}

SORE_Resource::Texture2D::Texture2D(std::string filename) : Resource(filename)
{
	handle = 0;
	Load();
}

GLuint SORE_Resource::Texture2D::GetHandle() const
{
	return handle;
}

void SORE_Resource::Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, handle);
}
