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

int SORE_Font::Print(font_ref fontIndex, int x, int y, const char* fmt, ...)
{
	if(fontIndex == 0) return 0;
	FontInfo* font = &fontStack[fontIndex];
	if(!font || font->height == 0) return INVALID_FONT_OBJ;
	
	
	GLuint fontList=font->listBase;
	float h=font->height/.63f;
	char text[256];
	va_list	ap;

	if (fmt == NULL)
		*text=0;
	else 
	{
		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);
	}
	
	const char* start_line=text;
	std::vector<std::string> lines;
	
	const char* c = text;
	
	for(;*c;c++) 
	{
		if(*c=='\n') 
		{
			std::string line;
			for(const char *n=start_line;n<c;n++) line.append(1,*n);
			lines.push_back(line);
			start_line=c+1;
		}
	}
	if(start_line) 
	{
		std::string line = start_line;
		lines.push_back(line);
	}
	
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

	glListBase(fontList);
	//int* listBase = new int[1];
	//glGetIntegerv(GL_LIST_BASE, listBase);
	//std::cout << "base: " << *listBase << std::endl;
	
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
	return 0;
}

float SORE_Font::Print(font_ref fontIndex, int x, int y, char c)
{
	if(fontIndex == 0) return 0.0;
	FontInfo* font = &fontStack[fontIndex];
	glEnd();
	//std::cout << "c: " << c << " x: " << x << " y: " << y << "\n";
	if(font->height == 0) return -1.0f;
	
	GLuint fontList=font->listBase;

	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	//glActiveTexture(GL_TEXTURE1);
	//glDisable(GL_TEXTURE_2D);
	//glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glListBase(fontList);
	int* listBase = NULL;
	glGetIntegerv(GL_LIST_BASE, listBase);
	
	float modelview_matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(static_cast<GLfloat>(x),static_cast<GLfloat>(y),0);
	glMultMatrixf(modelview_matrix);
	
	glCallLists(1, GL_UNSIGNED_BYTE, &c);
	
	float rpos[4];
	glGetFloatv(GL_CURRENT_RASTER_POSITION ,rpos);
	float len=x-rpos[0]; //(Assuming No Rotations Have Happend)

	//glPopMatrix();
	glPopAttrib();
	
	//PopProjectionMatrix();
	//glPopMatrix();
	return len;
}

SORE_Font::Font::Font(std::string filename, std::string fHeight) : Resource(filename, fHeight)
{
	height = boost::lexical_cast<unsigned int>(fHeight);
	Load();
}

SORE_Font::Font::~ Font()
{
}

void SORE_Font::Font::Load()
{
	if(height<=0) return INVALID_FONT_HEIGHT;
	textures = new GLuint[128];

	FT_Library library;
	if (FT_Init_FreeType( &library )) 
		return LIBRARY_LOAD_FAILED;

	FT_Face face;
	
	size_t length = strlen(fontName);
	if(length>59) return INVALID_FONT_NAME;
	if(fontPaths.size()==0) InitFontSystem();
	
	SORE_FileIO::file_ref fontObj = SORE_FileIO::Open(fileName.c_str());
	
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
		return FONT_LOAD_FAILED;
	}
	SORE_FileIO::Close(fontObj);
	if ((err=FT_New_Memory_Face( library, fontInfo, static_cast<FT_Long>(size), 0, &face ))!=0) 
	{
		FT_Done_FreeType(library);
		ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Font load failed: Freetype error code %d") % err);
		return FONT_LOAD_FAILED;
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
	
		return FONT_LOAD_FAILED;
	}
		
	glGenTextures( 128, textures );
		
	for(unsigned char i=0;i<128;i++)
		MakeDisplayList(face,i,listBase,textures);
	
	
	FT_Done_Face(face);
	FT_Done_FreeType(library);
	
	return 0;
}

void SORE_Font::Font::MakeDisplayList(FT_Face& face, char ch, )
{
	if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
		return GLYPH_LOAD_FAILED;

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
	
	glNewList(list_base+ch, GL_COMPILE);
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
