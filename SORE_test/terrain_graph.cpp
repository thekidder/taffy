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
#include <cstdio>

using namespace SORE_Math;

SORE_Graphics::TerrainGraph::TerrainGraph(int x, int y)
{
	xres = x;
	yres = y;
	pn = new Noise::PerlinNoise(Noise::Noise12D,0, 5,1);
	APP_LOG_S(SORE_Logging::LVL_DEBUG1,"Generating terrain...");
	cachedValues = new double[x*y];
	normalValues = new float[x*y*3];
	SORE_Math::Vector3D<double> temp;
	for(int i=0;i<xres;i++)
	{
		for(int j=0;j<yres;j++)
		{
			cachedValues[j + yres*i] = (pn->GetValue(i/100.0,j/100.0)+1.0)/2.0;
			
			if(cachedValues[j + yres*i]>1.0 || cachedValues[j + yres*i]<0.0)
				APP_LOG(SORE_Logging::LVL_WARNING, "Noise data is out of range: %f", cachedValues[j + j*i]);
		}
		if(int(float(i)/float(xres)*100.0)%5==0)
			APP_LOG(SORE_Logging::LVL_DEBUG2,"%5.1f percent done", float(i)/float(xres)*100.0);
	}
	APP_LOG_S(SORE_Logging::LVL_DEBUG1,"Generating normals...");
	for(int i=0;i<xres-1;i++)
	{
		for(int j=0;j<yres-1;j++)
		{
			int num = 0;
			Vector3D<float> total;
			Vector3D<float> v1 = Vector3D<float>(Point3D<float>(i,j,cachedValues[j + yres*i]));
			if(i<(xres-1) && j<(yres-1))
			{
				Vector3D<float> v2 = Vector3D<float>(Point3D<float>(i+1,j+1,cachedValues[j + yres*(i+1)+1]));
				v2 = v1.CrossProduct(v2).Normalize();
				if(v2.GetValue()[2]<cachedValues[j + yres*i])
					v2 = -v2;
				total += v2;
				num++;
			}
			total /= num;
			float* value = total.GetValue();
			normalValues[  (j + yres*i)*3] = value[0];
			normalValues[1+(j + yres*i)*3] = value[2];
			normalValues[2+(j + yres*i)*3] = value[1];
		}
		if(int(float(i)/float(xres)*100.0)%5==0)
			APP_LOG(SORE_Logging::LVL_DEBUG2,"%5.1f percent done", float(i)/float(xres)*100.0);
	}
	
	SORE_Resource::ResourceManager* rm = SORE_Resource::ResourceManager::GetManager();
	rm->RegisterLoader((SORE_Resource::RES_LOAD)SORE_Resource::LoadTexture, "tga");
	rm->Register("data/Textures/crate.tga");
	wireframe = false;
	normals = true;
}

SORE_Graphics::TerrainGraph::~TerrainGraph()
{
	delete pn;
	delete cachedValues;
	delete normalValues;
}
	
void SORE_Graphics::TerrainGraph::Render()
{
	SORE_Resource::ResourceManager* rm = SORE_Resource::ResourceManager::GetManager();
	SORE_Resource::Resource* re;
	SORE_Resource::ResourceHandle* rd;
	
	const float vscale = 6.0*scale;
	const GLfloat LightPosition[]= {(xres/2.0f)*scale, 3.0f, (yres/2.0f)*scale+0.5f};
	const GLfloat lightPos[] = {0.0f, 0.0f, 0.0f, 1.0f};
	const GLfloat LightAmbient[]=  {  0.01f, 0.01f, 0.01f, 1.0f };
	const GLfloat LightDiffuse[]=  {  0.6f, 0.6f, 0.6f, 1.0f };
	
	glEnable(GL_DEPTH_TEST); 
	
	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.5);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.1);
	glDisable(GL_TEXTURE_2D);
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_AUTO_NORMAL);
	//glEnable(GL_TEXTURE_2D);
	glColorMaterial ( GL_FRONT, GL_AMBIENT ) ;
	glEnable ( GL_COLOR_MATERIAL ) ;
	glPushMatrix();
	//glTranslatef(LightPosition[0], LightPosition[1], LightPosition[2]);
	//glDisable(GL_LIGHTING);
	//glDisable( GL_COLOR_MATERIAL ) ;
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
	//glEnable(GL_LIGHTING);
	//glEnable ( GL_COLOR_MATERIAL ) ;
	
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glPopMatrix();
	//re = rm->GetPtr("data/Textures/crate.tga");
	//rd = dynamic_cast<SORE_Resource::ResourceHandle*>(re);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//glBindTexture( GL_TEXTURE_2D, rd->GetHandle());
	for(int i=0;i<xres-1;i++)
	{
		GLfloat normal[3];
		normal[0] = 0.0f;
		normal[1] = 1.0f;
		normal[2] = 0.0f;
		if(wireframe)
			glBegin(GL_LINE_STRIP);
		else
			glBegin(GL_TRIANGLE_STRIP);
		for(int j=0;j<yres;j++)
		{
			//if(j%2==0)
			//	glTexCoord2f(1.0f, 0.0f);
			//else
			//	glTexCoord2f(1.0f, 1.0f);
			glNormal3fv(normal);
			glColor4f(cachedValues[j + yres*i], cachedValues[j + yres*i], cachedValues[j + yres*i], 1.0f);
			glVertex3f(scale*i,vscale*cachedValues[j + yres*i], scale*j);
			//if(j%2==0)
			//	glTexCoord2f(0.0f, 0.0f);
			//else
			//	glTexCoord2f(0.0f, 1.0f);
			glNormal3fv(normal);
			glColor4f(cachedValues[j + yres*(i+1)], cachedValues[j + yres*(i+1)], cachedValues[j + yres*(i+1)], 1.0f);
			glVertex3f(scale*i+scale,vscale*cachedValues[j + yres*(i+1)],scale*j);
		}
		glEnd();
	}
	/*for(int i=0;i<xres-1;i++)
	{
		for(int j=0;j<yres-1;j++)
		{
			if(wireframe)
				glBegin(GL_LINES);
			else
				glBegin(GL_TRIANGLES);
			glNormal3fv(&normalValues[(j + yres*i)*3]);
			glColor4f(cachedValues[j + yres*i], cachedValues[j + yres*i], cachedValues[j + yres*i], 1.0f);
			glVertex3f(scale*i,vscale*cachedValues[j + yres*i], scale*j);
			
			glNormal3fv(&normalValues[(j + yres*(i+1))*3]);
			glColor4f(cachedValues[j + yres*(i+1)], cachedValues[j + yres*(i+1)], cachedValues[j + yres*(i+1)], 1.0f);
			glVertex3f(scale*i+scale,vscale*cachedValues[j + yres*(i+1)],scale*j);
					
			glNormal3fv(&normalValues[(j + yres*(i+1)+1)*3]);
			glColor4f(cachedValues[j + yres*(i+1)+1], cachedValues[j + yres*(i+1)+1], cachedValues[j + yres*(i+1)+1], 1.0f);
			glVertex3f(scale*i+scale,vscale*cachedValues[j + yres*(i+1)+1],scale*j+scale);
			glEnd();
			
			if(wireframe)
				glBegin(GL_LINES);
			else
				glBegin(GL_TRIANGLES);
			glNormal3fv(&normalValues[(j + yres*i)*3]);
			glColor4f(cachedValues[j + yres*i], cachedValues[j + yres*i], cachedValues[j + yres*i], 1.0f);
			glVertex3f(scale*i,vscale*cachedValues[j + yres*i], scale*j);
		
			glNormal3fv(&normalValues[(j + yres*i+1)*3]);
			glColor4f(cachedValues[j + yres*i+1], cachedValues[j + yres*i+1], cachedValues[j + yres*i+1], 1.0f);
			glVertex3f(scale*i,vscale*cachedValues[j + yres*i+1],scale*j+scale);
					
			glNormal3fv(&normalValues[(j + yres*(i+1)+1)*3]);
			glColor4f(cachedValues[j + yres*(i+1)+1], cachedValues[j + yres*(i+1)+1], cachedValues[j + yres*(i+1)+1], 1.0f);
			glVertex3f(scale*i+scale,vscale*cachedValues[j + yres*(i+1)+1],scale*j+scale);
			glEnd();
		}
	}
	*/
	
	if(normals)
	{
		glDisable(GL_LIGHTING);
		for(int i=0;i<xres;i++)
		{
			for(int j=0;j<yres;j++)
			{
				
				glBegin(GL_LINES);
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(scale*i,vscale*cachedValues[j + yres*i], scale*j);
				glVertex3f(scale*i+normalValues[(j + yres*i)*3],vscale*cachedValues[j + yres*i]+normalValues[(j + yres*i)*3+1], scale*j+normalValues[(j + yres*i)*3+2]);
				glEnd();
			}
		}
	}
}

void SORE_Graphics::TerrainGraph::WritePGM(const char* name)
{
	char buffer[32];
	FILE* file = fopen(name, "w");
	fputs("P2\n", file);
	sprintf(buffer, "%d %d\n", xres, yres);
	fputs(buffer, file);
	fputs("255\n", file);
	for(int i=0;i<xres*yres;i++)
	{
		int value = int(cachedValues[i]*255);
		if(i%(xres-1)!=0 || i==0)
			sprintf(buffer, "%d ", value);
		else
			sprintf(buffer, "%d\n", value);
		if(value<0 || value>255)
			APP_LOG(SORE_Logging::LVL_WARNING, "value out of range (%d) at %d", value, i);
		fputs(buffer, file);
	}
	fclose(file);
}
