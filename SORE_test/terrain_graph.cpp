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
#include "main.h"

SORE_Graphics::TerrainGraph::TerrainGraph(int x, int y)
{
	xres = x;
	yres = y;
	pn = new Noise::PerlinNoise(Noise::Noise12D,0,5,0.25);
	APP_LOG_S(SORE_Logging::LVL_DEBUG1,"Generating terrain...");
	cachedValues = new double[x*y];
	for(int i=0;i<xres;i++)
	{
		for(int j=0;j<yres;j++)
		{
			cachedValues[j + j*i] = pn->GetValue(i,j);
		}
		APP_LOG(SORE_Logging::LVL_DEBUG1,"%f percent done", float(i)/float(xres)*100.0);
	}
}

SORE_Graphics::TerrainGraph::~TerrainGraph()
{
	delete pn;
	delete cachedValues;
}
	
void SORE_Graphics::TerrainGraph::Render()
{
	const float scale = 0.01f;
	const float vscale = 0.02f;
	/*const GLfloat LightPosition[]= {(xres/2.0f)*scale, 3.0f, (yres/2.0f)*scale+0.5f, 1.0f };
	const GLfloat LightAmbient[]=  {  0.01f, 0.01f, 0.01f, 1.0f };
	const GLfloat LightDiffuse[]=  {  0.6f, 0.6f, 0.6f, 1.0f };
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT ) ;
	glEnable ( GL_COLOR_MATERIAL ) ;
	glPushMatrix();
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glPopMatrix();*/
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	float z;
	for(int i=0;i<xres-1;i++)
	{
		glBegin(GL_LINE_STRIP);
		for(int j=0;j<yres;j++)
		{
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glVertex3f(scale*i,vscale*cachedValues[j + j*i], scale*j);
			//glColor4f(0.0f, 0.8f, 0.0f, 1.0f);
			glVertex3f(scale*i+scale,vscale*cachedValues[j + j*(i+1)],scale*j);
			//APP_LOG(SORE_Logging::LVL_DEBUG2, "(%f, %f, %f)",scale*i,vscale*cachedValues[j + j*i], scale*j);
		}
		glEnd();
	}
}
