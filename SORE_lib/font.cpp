#include "font.h"
#include "fileio.h"
#include "sore_graphics.h"

namespace SORE_Font
{
	static vector<string> fontPaths;
	static vector<FontInfo> fontStack;
}

inline int next_p2 (int a )
{
	int rval=1;
	while(rval<a) rval<<=1;
	return rval;
}

int SORE_Font::MakeDisplayList( FT_Face face, char ch, GLuint list_base, GLuint * tex_base )
{
	if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
		return GLYPH_LOAD_FAILED;
	
	// Move The Face's Glyph Into A Glyph Object.
	FT_Glyph glyph;
	if(FT_Get_Glyph( face->glyph, &glyph ))
		return GET_GLYPH_FAILED;
	
	FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
	
	FT_Bitmap& bitmap=bitmap_glyph->bitmap;
	
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
	glBindTexture( GL_TEXTURE_2D, tex_base[ch]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
				  GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );
	delete [] expanded_data;
	
	glNewList(list_base+ch, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, tex_base[ch]);
	glPushMatrix();
	glTranslatef(bitmap_glyph->left,0.0f,0.0f);
	
	glTranslatef(0.0f,bitmap_glyph->top-bitmap.rows,0.0f);
	float x=(float)bitmap.width / (float)width, y=(float)bitmap.rows / (float)height;
	
	glBegin(GL_QUADS);
	glTexCoord2d(0,0); glVertex2f(0.0f,bitmap.rows);
	glTexCoord2d(0,y); glVertex2f(0.0f,0.0f);
	glTexCoord2d(x,y); glVertex2f(bitmap.width,0.0f);
	glTexCoord2d(x,0); glVertex2f(bitmap.width,bitmap.rows);
	glEnd();
	glPopMatrix();
	glTranslatef(face->glyph->advance.x >> 6 ,0.0f,0.0f);
	glEndList();
	return 0;
}

int SORE_Font::FontInfo::Init()
{	
	height=0;
	
	return 0;
}

int SORE_Font::FontInfo::Init(const char* fontName, unsigned int h)
{	
	height = h;
	return LoadFont(fontName);
}

void SORE_Font::FontInfo::Cleanup()
{
	glDeleteLists(listBase,128);
	glDeleteTextures(128,textures);
	if(textures)
		delete [] textures;
	if(fontInfo)
		delete [] fontInfo;
}

/*inline void SORE_Font::PushScreenCoordMatrix()
{
	glPushAttrib(GL_TRANSFORM_BIT);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(viewport[0],viewport[2],viewport[1],viewport[3]);
	glPopAttrib();
}

inline void SORE_Font::PopProjectionMatrix()
{
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}*/

int SORE_Font::FontHeight(font_ref font)
{
	FontInfo* fontObj = &fontStack[font];
	return fontObj->height;
}

int SORE_Font::Print(font_ref fontIndex, int x, int y, const char* fmt, ...)
{
	//std::cout << "test\n";
	FontInfo* font = &fontStack[fontIndex];
	//std::cout << "font.h: " << font.height << std::endl;
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
	vector<string> lines;
	
	const char* c = text;
	
	for(;*c;c++) 
	{
		if(*c=='\n') 
		{
			string line;
			for(const char *n=start_line;n<c;n++) line.append(1,*n);
			lines.push_back(line);
			start_line=c+1;
		}
	}
	if(start_line) 
	{
		string line;
		for(const char *n=start_line;n<c;n++) line.append(1,*n);
		lines.push_back(line);
	}
	
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glListBase(fontList);
	//int* listBase = new int[1];
	//glGetIntegerv(GL_LIST_BASE, listBase);
	//std::cout << "base: " << *listBase << std::endl;
	
	float modelview_matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	
	for(int i=0;i<lines.size();i++) 
	{
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(x,y-h*i,0);
		glMultMatrixf(modelview_matrix);
		//glScalef(0.01f, 0.01f, 0.01f);
		glCallLists(lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());
		// float rpos[4];
		// glGetFloatv(GL_CURRENT_RASTER_POSITION ,rpos);
		// float len=x-rpos[0]; //(Assuming No Rotations Have Happend)

		glPopMatrix();
	}
	glPopAttrib();
}

float SORE_Font::Print(font_ref fontIndex, int x, int y, char c)
{
	FontInfo* font = &fontStack[fontIndex];
	glEnd();
	std::cout << "c: " << c << " x: " << x << " y: " << y << "\n";
	if(font->height == 0) return -1.0f;
	
	GLuint fontList=font->listBase;

	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glListBase(fontList);
	int* listBase;
	glGetIntegerv(GL_LIST_BASE, listBase);
	std::cout << "base: " << *listBase << std::endl;
	
	float modelview_matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x,y,0);
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

int SORE_Font::FontInfo::LoadFont(const char* fontName)
{
	if(height<=0) return INVALID_FONT_HEIGHT;
	//if(textures) delete[] textures;
	textures = new GLuint[128];
	FT_Library library;
	if (FT_Init_FreeType( &library )) 
		return LIBRARY_LOAD_FAILED;
	
	FT_Face face;
	
	int length = strlen(fontName);
	if(length>59) return INVALID_FONT_NAME;
	if(fontPaths.size()==0) Init();
	
	char* dirName = new char[512];
	char*  fontPath = new char[2048];
	char  fileName[64];
	
	strcpy(fileName, fontName);
	
	if(!strcmp((fontName+length-3), ".ttf"))
	{
		strcat(fileName, ".ttf");
	}
	
	//FILE* fontObj;
	
	SORE_FileIO::file_ref fontObj = SORE_FileIO::Open(fileName);
	

	//fontObj = fopen(fileName, "r");
	if(fontObj==0)
	{
		std::vector<std::string>::iterator it;
		for(it=fontPaths.begin();it<fontPaths.end();it++)
		{
			strcpy(fontPath, it->c_str());
			strcat(fontPath, fileName);
			fontObj = SORE_FileIO::Open(fontPath);
			if(fontObj!=0)
			{
				break;
			}
			strcpy(fontPath, "");
			fontObj = NULL;
		}
	}
	else
	{
		strcpy(fontPath, fileName);
	}
	if(strlen(fontPath)==0) return INVALID_FONT_NAME;
	
	unsigned int size = SORE_FileIO::Size(fontObj);
	int err;
	
	fontInfo = new FT_Byte[size];
	
	if((err=SORE_FileIO::Read(fontInfo, 1, size, fontObj))!=size)
	{
		std::cerr << "Font load failed: Could not read font from disk (expected " << size << " bytes, read " << err << " bytes)\n";
		SORE_FileIO::Close(fontObj);
		return FONT_LOAD_FAILED;
	}
	SORE_FileIO::Close(fontObj);
	if ((err=FT_New_Memory_Face( library, fontInfo, size, 0, &face ))!=0) 
	{
		delete[] fontPath;
		FT_Done_FreeType(library);
		delete[] dirName;
		std::cerr << "Font load failed: Freetype error code " << err << std::endl;
		return FONT_LOAD_FAILED;
	}
	
	FT_Set_Char_Size( face, height << 6, height << 6, 96, 96);
	
	listBase=glGenLists(128);
	GLenum error;
	while((error=glGetError())!=GL_NO_ERROR)
	{
		std::cout << "error detected: " << error << "\n";
		delete[] fontPath;
	
		FT_Done_Face(face);
		FT_Done_FreeType(library);
	
		delete[] dirName;
		return FONT_LOAD_FAILED;
	}
		
	glGenTextures( 128, textures );
	
	//std::cout << fontPath << "\n";
	
	for(unsigned char i=0;i<128;i++)
		MakeDisplayList(face,i,listBase,textures);
	
	delete[] fontPath;
	
	FT_Done_Face(face);
	FT_Done_FreeType(library);
	
	delete[] dirName;
	return 0;
}

int SORE_Font::FontInfo::SetHeight(unsigned int h)
{
	height = h;
	return 0;
}

int SORE_Font::Init()
{
	SORE_Font::fontPaths.clear();
#ifndef WIN32
	if(!FcInit())
	{
		return LIBRARY_LOAD_FAILED;
	}
	FcConfig* config = FcConfigGetCurrent();
	FcStrList* fontDirs = FcConfigGetFontDirs(config);
#endif
	
	char fontPath[2048];
	char* dirName = new char[2048];
	
#ifdef WIN32
	char windows_path[MAX_PATH];
	GetWindowsDirectory(windows_path, MAX_PATH);
	strcpy(fontPath, windows_path);
	strcat(fontPath, "\\Fonts\\");
	fontPaths.push_back(fontPath);
#else
	while(dirName = (char*)FcStrListNext(fontDirs))
	{
		strcpy(fontPath, dirName);
		strcat(fontPath, "/");
		fontPaths.push_back(fontPath);
	}
#endif
#ifndef WIN32
	FcStrListDone(fontDirs);
#endif
	delete[] dirName;
}

SORE_Font::font_ref SORE_Font::LoadFont(const char* font, unsigned int h)
{
	int err;
	FontInfo newFont;
	newFont.SetHeight(h);
	if((err=newFont.LoadFont(font))!=0)
	{
		std::cerr << "Failed to load font; error code " << err << std::endl;
		return 0;
	}
	font_ref index = fontStack.size();
	fontStack.push_back(newFont);
	return index;
}

void SORE_Font::Cleanup()
{
	for(int i=0;i<fontStack.size();i++)
	{
		fontStack[i].Cleanup();
	}
}
