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

#ifndef  __TERRAIN_GRAPH_H__
#define  __TERRAIN_GRAPH_H__

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
			void WritePGM(const char* name);
			void ToggleWireframe() {wireframe = !wireframe;}
		protected:
			int xres,yres;
			Noise::PerlinNoise* pn;
			double* cachedValues;
			float* normalValues;
			const static float scale = 1.0f;
			bool wireframe;
	};
}

#endif /*__TERRAIN_GRAPH_H__*/
