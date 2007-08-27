//
// C++ Implementation: terrain_graph
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "terrain_graph.h"

SORE_Graphics::TerrainGraph::TerrainGraph(int x, int y)
{
	xres = x;
	yres = y;
	pn = new Noise::PerlinNoise(Noise::Noise12D,0,5,0.5);
}

SORE_Graphics::TerrainGraph::~TerrainGraph()
{
	delete pn;
}
	
void SORE_Graphics::TerrainGraph::Render()
{
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	float z;
	for(int i=0;i<xres;i++)
	{
		glBegin(GL_LINE_STRIP);
		for(int j=0;j<yres;j++)
		{
			//glVertex3f(i,pn->GetValue(i,j), j);
			//glVertex3f(i+1,pn->GetValue(i,j),j);
			glVertex3f(0.1f*i,0.1f*pn->GetValue(i,j), 0.1f*j);
			glVertex3f(0.1f*i+0.1f,0.1f*pn->GetValue(i+1,j),0.1f*j);
		}
		glEnd();
	}
}
