// $Id$

#include "sore_font.h"
#include "sore_fileio.h"
#include "sore_logger.h"

#include <vector>
#include <string>
#include <string.h>
#include <cstdlib>

#include <boost/lexical_cast.hpp>

inline int next_p2 (int a )
{
	int rval=1;
	while(rval<a) rval<<=1;
	return rval;
}

std::vector<std::string> SORE_Font::Font::fontPaths;

SORE_Font::Font::Font(std::string filename, std::string fHeight) : Resource(filename)
{
	height = boost::lexical_cast<unsigned int>(fHeight);
	if(fontPaths.size()==0) InitPaths();
	Load();
}

SORE_Font::Font::~ Font()
{
}

void SORE_Font::Font::Load()
{
	if(height<=0) return;
	textures = new GLuint[128];

	FT_Library library;
	if (FT_Init_FreeType( &library )) 
		return;

	FT_Face face;
	
	size_t length = filename.size();
	if(length>59) return;
	
	SORE_FileIO::file_ref fontObj = SORE_FileIO::Open(filename.c_str());
	
	if(fontObj==0)
	{
		std::vector<std::string>::iterator it;
		for(it=fontPaths.begin();it<fontPaths.end();it++)
		{
			std::string path = *it;
			path += filename;
			fontObj = SORE_FileIO::Open(path.c_str());
			if(fontObj!=0)
			{
				break;
			}
			fontObj = 0;
		}
	}
		
	size_t size = SORE_FileIO::Size(fontObj);
	size_t err;
	
	FT_Byte* fontInfo = new FT_Byte[size];
	
	if((err=SORE_FileIO::Read(fontInfo, 1, size, fontObj))!=size)
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Font load failed: Could not read font from disk (expected %d bytes, read %d bytes)") % size % err);
		SORE_FileIO::Close(fontObj);
		return;
	}
	SORE_FileIO::Close(fontObj);
	if ((err=FT_New_Memory_Face( library, fontInfo, static_cast<FT_Long>(size), 0, &face ))!=0) 
	{
		FT_Done_FreeType(library);
		ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Font load failed: Freetype error code %d") % err);
		return;
	}
	
	FT_Set_Pixel_Sizes( face, 0, height);
	
	listBase=glGenLists(128);
	GLenum error;
	if((error=glGetError())!=GL_NO_ERROR)
	{
		do
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("GL Error: %d") % error);
		} while((error=glGetError())!=GL_NO_ERROR);
	
		FT_Done_Face(face);
		FT_Done_FreeType(library);
	
		return;
	}
		
	glGenTextures( 128, textures );
		
	for(unsigned char i=0;i<128;i++)
		MakeDisplayList(face,i);
	
	
	FT_Done_Face(face);
	FT_Done_FreeType(library);
	
	return;
}

void SORE_Font::Font::MakeDisplayList(FT_Face& face, char ch)
{
	if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
		return;

	FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	FT_Bitmap& bitmap=face->glyph->bitmap;
	
	int width = next_p2( bitmap.width );
	int height = next_p2( bitmap.rows );

	GLubyte* expanded_data = new GLubyte[ 2 * width * height];
	
	for(int j=0; j<height;j++)
	{
		for(int i=0; i < width; i++)
		{
			expanded_data[2*(i+j*width)] = expanded_data[2*(i+j*width)+1] = 
					(i>=bitmap.width || j>=bitmap.rows) ?
					0 : bitmap.buffer[i + bitmap.width*j];
		}
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, textures[ch]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_INTENSITY, width, height, 0,
								GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );
	delete [] expanded_data;
	
	glNewList(listBase+ch, GL_COMPILE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[ch]);
	glPushMatrix();
		
	glTranslatef(static_cast<GLfloat>(face->glyph->bitmap_left),0.0f,0.0f);
	glTranslatef(0.0f,static_cast<GLfloat>(face->glyph->bitmap_top-bitmap.rows), 0.0f);
	GLfloat x=static_cast<GLfloat>(bitmap.width) / static_cast<GLfloat>(width), y=static_cast<GLfloat>(bitmap.rows) / static_cast<GLfloat>(height);
	
	glBegin(GL_QUADS);
	glTexCoord2d(0,0); glVertex2f(0.0f,static_cast<GLfloat>(bitmap.rows));
	glTexCoord2d(0,y); glVertex2f(0.0f,0.0f);
	glTexCoord2d(x,y); glVertex2f(static_cast<GLfloat>(bitmap.width),0.0f);
	glTexCoord2d(x,0); glVertex2f(static_cast<GLfloat>(bitmap.width),static_cast<GLfloat>(bitmap.rows));
	glEnd();
	glPopMatrix();
	glTranslatef(static_cast<GLfloat>(face->glyph->advance.x >> 6) ,0.0f,0.0f);
	glEndList();
}

unsigned int SORE_Font::Font::Height() const
{
	return height;
}

float SORE_Font::Font::Print(int x, int y, std::string str)
{	
	float h=height/.63f;

	std::vector<std::string> lines;
	
	const char* c = str.c_str();
	unsigned int start = 0;
	
	for(;*c;c++) 
	{
		if(*c=='\n') 
		{
			std::string line;
			line = str.substr(start, c-str.c_str());
			//for(const char *n=start_line;n<c;n++) line.append(1,*n);
			lines.push_back(line);
			start=c-str.c_str();
		}
	}
	if(start == 0) lines.push_back(str);

	
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE4);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE5);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE6);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE7);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glListBase(listBase);
	
	float modelview_matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	
	for(size_t i=0;i<lines.size();i++)
	{
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(static_cast<GLfloat>(x),static_cast<GLfloat>(y-h*i),0);
		glMultMatrixf(modelview_matrix);
		glCallLists(static_cast<GLsizei>(lines[i].length()), GL_UNSIGNED_BYTE, lines[i].c_str());
		// float rpos[4];
		// glGetFloatv(GL_CURRENT_RASTER_POSITION ,rpos);
		// float len=x-rpos[0]; //(Assuming No Rotations Have Happend)

		glPopMatrix();
	}
	glPopAttrib();
	return 0.0;
}

float SORE_Font::Font::Print(int x, int y, boost::format str)
{
	return Print(x, y, boost::str(str));
}
