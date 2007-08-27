//
// C++ Interface: terrain_graph
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <sore.h>
#include "perlin.h"

namespace SORE_Graphics
{
	class TerrainGraph : public SceneGraph
	{
		public:
			TerrainGraph(int x, int y);
			~TerrainGraph();
		
			void Render();
		protected:
			int xres,yres;
			Noise::PerlinNoise* pn;
	};
}
