#include "sore_allgl.h"
#include "sore_font.h"

#include <string>

using SORE_Font::font_ref;

namespace SORE_Util
{
	class Console
	{
		public:
			Console(font_ref fontID = -1, int x = 0, int y = 0, int width = 5.0, unsigned int height = 2.0);
			void Render();
			void PutChar(char c);
			char GetChar();
			void InitFont(font_ref fontID);
		protected:
			int x, y;
			float xPos, yPos;
			int width, height;
			font_ref consoleFont;
			std::string outbuffer;
			unsigned int numLines;
	};
}
