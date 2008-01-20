// $Id$

#include "sore_console.h"
#include "sore_allgl.h"
#include "sore_graphics.h"

SORE_Utility::Console::Console(font_ref fontID, int x, int y, int width, unsigned int height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	xPos = yPos = 4.0f;
	//consoleFont.SetHeight(16);
	//std::cout << "load font returned: " << consoleFont.LoadFont(font) << std::endl;
	//consoleFont = FontSystem::LoadFont(font, height);
	consoleFont = fontID;
	numLines = 1;
}

void SORE_Utility::Console::InitFont(font_ref fontID)
{
	consoleFont = fontID;
}

void SORE_Utility::Console::Render()
{
	glColor3f(1.0f, 1.0f, 1.0f);
	//glEnable(GL_SCISSOR_TEST);
	//SORE_2DOverlay::overlay_settings settings = SORE_2DOverlay::Get_Window();
	GLint   viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	//glScissor(x/settings.xMin*viewport[0],y/settings.yMin*viewport[2], width/settings.xMax*viewport[1], height/settings.yMax*viewport[3]);
	//glScissor(viewport[0], viewport[2], viewport[1], viewport[3]);
	SORE_Font::Print(consoleFont, int(x+xPos), int(y-yPos), outbuffer.c_str());
	//glDisable(GL_SCISSOR_TEST);
}

void SORE_Utility::Console::PutChar(char c)
{
	outbuffer += c;
	if(c=='\n') numLines++;
}

char SORE_Utility::Console::GetChar()
{
	return ' ';
}
