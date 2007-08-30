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
	const float scale = 1.0f;
	const float vscale = 6.0*scale;
	class TerrainGraph : public SceneGraph, public SORE_Kernel::Task
	{
		public:
			TerrainGraph(int x, int y);
			~TerrainGraph();
		
			void Render();
			void WritePGM(const char* name);
			void ToggleWireframe() {wireframe = !wireframe;}
			void ToggleNormals() {normals = !normals;}
			void ToggleHeightmapColoring() {heightColor = !heightColor;}
			bool LightMoveCallback(SORE_Kernel::Event* event);
			
			
			void Pause() {}
			void Resume() {}
			void Frame(int elapsedTime);
			
			const char* GetName() const {return "TerrainGraph input task";}
			
		protected:
			int xres,yres;
			Noise::PerlinNoise* pn;
			double* cachedValues;
			float* normalValues;
			GLfloat LightPosition[3];
			bool wireframe,normals, heightColor;
			float lightMoveX, lightMoveY, lightMoveZ;
	};
}

#endif /*__TERRAIN_GRAPH_H__*/
