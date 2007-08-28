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
	pn = new Noise::PerlinNoise(Noise::Noise12D,0,10,0.002);
	APP_LOG_S(SORE_Logging::LVL_DEBUG1,"Generating terrain...");
	cachedValues = new double[x*y];
	for(int i=0;i<xres;i++)
	{
		for(int j=0;j<yres;j++)
		{
			cachedValues[j + j*i] = pn->GetValue(i,j);
		}
		APP_LOG(SORE_Logging::LVL_DEBUG2,"%5.1f percent done", float(i)/float(xres)*100.0);
	}
	
	SORE_Resource::ResourceManager* rm = SORE_Resource::ResourceManager::GetManager();
	rm->RegisterLoader((SORE_Resource::RES_LOAD)SORE_Resource::LoadTexture, "tga");
	rm->Register("data/Textures/crate.tga");
}

SORE_Graphics::TerrainGraph::~TerrainGraph()
{
	delete pn;
	delete cachedValues;
}
	
void SORE_Graphics::TerrainGraph::Render()
{
	SORE_Resource::ResourceManager* rm = SORE_Resource::ResourceManager::GetManager();
	SORE_Resource::Resource* re;
	SORE_Resource::ResourceHandle* rd;
	const float scale = 0.1f;
	const float vscale = 0.2f;
	const GLfloat LightPosition[]= {(xres/2.0f)*scale, 3.0f, (yres/2.0f)*scale+0.5f};
	const GLfloat lightPos[] = {0.0f, 0.0f, 0.0f, 1.0f};
	const GLfloat LightAmbient[]=  {  0.01f, 0.01f, 0.01f, 1.0f };
	const GLfloat LightDiffuse[]=  {  0.6f, 0.6f, 0.6f, 1.0f };
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT ) ;
	glEnable ( GL_COLOR_MATERIAL ) ;
	glPushMatrix();
	glTranslatef(LightPosition[0], LightPosition[1], LightPosition[2]);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable( GL_COLOR_MATERIAL ) ;
	glBegin(GL_LINE_LOOP);
		
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glVertex3f(-0.1f, -0.1f,  0.1f); //bottom
	glVertex3f( 0.1f, -0.1f,  0.1f);
	glVertex3f( 0.1f, -0.1f, -0.1f);
	glVertex3f(-0.1f, -0.1f, -0.1f);
	
	glEnd();
	glBegin(GL_LINE_LOOP);
		
	glVertex3f(-0.1f,  0.1f,  0.1f); //top
	glVertex3f( 0.1f,  0.1f,  0.1f);
	glVertex3f( 0.1f,  0.1f, -0.1f);
	glVertex3f(-0.1f,  0.1f, -0.1f);
		
	glEnd();
	glBegin(GL_LINE_LOOP);
	
		//front
	glVertex3f(-0.1f, -0.1f,  0.1f); //bottom left
	glVertex3f(-0.1f,  0.1f,  0.1f); //top left
	glVertex3f( 0.1f,  0.1f,  0.1f); //top right
	glVertex3f( 0.1f, -0.1f,  0.1f); //bottom right
	
	glEnd();
	glBegin(GL_LINE_LOOP);
		
	glVertex3f(-0.1f, -0.1f, -0.1f); //back
	glVertex3f(-0.1f,  0.1f, -0.1f);
	glVertex3f( 0.1f,  0.1f, -0.1f);
	glVertex3f( 0.1f, -0.1f, -0.1f);
	
	glEnd();
	glBegin(GL_LINE_LOOP);
			
	glVertex3f(-0.1f, -0.1f,  0.1f); //left
	glVertex3f(-0.1f,  0.1f,  0.1f);
	glVertex3f(-0.1f,  0.1f, -0.1f);
	glVertex3f(-0.1f, -0.1f, -0.1f);
	
	glEnd();
	glBegin(GL_LINE_LOOP);
		
	glVertex3f( 0.1f, -0.1f,  0.1f); //right
	glVertex3f( 0.1f,  0.1f,  0.1f);
	glVertex3f( 0.1f,  0.1f, -0.1f);
	glVertex3f( 0.1f, -0.1f, -0.1f);
		
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable ( GL_COLOR_MATERIAL ) ;
	
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
	glPopMatrix();
	//float z;
	re = rm->GetPtr("data/Textures/crate.tga");
	rd = dynamic_cast<SORE_Resource::ResourceHandle*>(re);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture( GL_TEXTURE_2D, rd->GetHandle());
	for(int i=0;i<xres-1;i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(int j=0;j<yres;j++)
		{
			//glColor4f(cachedValues[j + j*i], cachedValues[j + j*i], cachedValues[j + j*i], 1.0f);
			/*if(j%2==0)
				glTexCoord2f(1.0f, 0.0f);
			else
				glTexCoord2f(1.0f, 1.0f);*/
			glVertex3f(scale*i,vscale*cachedValues[j + j*i], scale*j);
			//glColor4f(cachedValues[j + j*i], cachedValues[j + j*i], cachedValues[j + j*i], 1.0f);
			/*if(j%2==0)
				glTexCoord2f(0.0f, 0.0f);
			else
				glTexCoord2f(0.0f, 1.0f);*/
			glVertex3f(scale*i+scale,vscale*cachedValues[j + j*(i+1)],scale*j);
		}
		glEnd();
	}
	/*for(int i=0;i<xres-1;i++)
	{
		for(int j=0;j<yres-1;j++)
		{
			glBegin(GL_TRIANGLES);
			//glTexCoord2f(0.0f, 1.0f);
			glVertex3f(scale*i,vscale*cachedValues[j + j*i], scale*j);
			//glTexCoord2f(0.0f, 0.0f);
			glVertex3f(scale*i+scale,vscale*cachedValues[j + j*(i+1)],scale*j);
			//glTexCoord2f(1.0f, 0.0f);
			glVertex3f(scale*i,vscale*cachedValues[j + j*i + 1],scale*j+scale);
			
			glEnd();
		}
	}*/
}
