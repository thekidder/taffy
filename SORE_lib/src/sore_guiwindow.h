// $Id$

#include "sore_geometry.h"

namespace SORE_GUI
{
	class Window
	{
		public:
			virtual SORE_Math::Rect<unsigned int> BoundingBox() = 0; //bounding box in screen pixels
	};
}