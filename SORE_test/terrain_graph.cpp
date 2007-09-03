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
	APP_LOG_S(SORE_Logging::LVL_DEBUG2, "In TerrainGraph constructor");
	mainLog->Flush();
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
	for(int i=0;i<xres;i++)
	{
		for(int j=0;j<yres;j++)
		{
			int num = 0;
			Vector3D<float> total,v2,v3;
			Point3D<float> p1(i,vscale*cachedValues[j + yres*i],j);
			if(i<(xres-1) && j<(yres-1))
			{
				v2 = Vector3D<float>(Point3D<float>(i+1,vscale*cachedValues[j + yres*(i+1)],j)-p1);
				v3 = Vector3D<float>(Point3D<float>(i,vscale*cachedValues[j + yres*i+1],j+1)-p1);
				v2 = v2.CrossProduct(v3).Normalize();
				if(v2.GetValue()[1]<cachedValues[j + yres*i])
					v2 = -v2;
				total += v2;
				num++;
			}
			if(i>0 && j<(yres-1))
			{
				v2 = Vector3D<float>(Point3D<float>(i-1,vscale*cachedValues[1 + j + yres*(i-1)],j+1)-p1);
				v3 = Vector3D<float>(Point3D<float>(i,vscale*cachedValues[j + yres*i+1],j+1)-p1);
				v2 = v2.CrossProduct(v3).Normalize();
				if(v2.GetValue()[1]<cachedValues[j + yres*i])
					v2 = -v2;
				total += v2;
				num++;
				
				v2 = Vector3D<float>(Point3D<float>(i-1,vscale*cachedValues[1 + j + yres*(i-1)],j+1)-p1);
				v3 = Vector3D<float>(Point3D<float>(i-2,vscale*cachedValues[j + yres*(i-1)],j)-p1);
				v2 = v2.CrossProduct(v3).Normalize();
				if(v2.GetValue()[1]<cachedValues[j + yres*i])
					v2 = -v2;
				total += v2;
				num++;
			}
			if(i>0 && j>0)
			{
				v2 = Vector3D<float>(Point3D<float>(i-1,vscale*cachedValues[j + yres*(i-1)],j)-p1);
				v3 = Vector3D<float>(Point3D<float>(i,vscale*cachedValues[j + yres*i-1],j-1)-p1);
				v2 = v2.CrossProduct(v3).Normalize();
				if(v2.GetValue()[1]<cachedValues[j + yres*i])
					v2 = -v2;
				total += v2;
				num++;
			}
			if(i<(xres-1) && j>0)
			{
				v2 = Vector3D<float>(Point3D<float>(i+1,vscale*cachedValues[j + yres*(i+1)-1],j-1)-p1);
				v3 = Vector3D<float>(Point3D<float>(i,vscale*cachedValues[j + yres*i-1],j-1)-p1);
				v2 = v2.CrossProduct(v3).Normalize();
				if(v2.GetValue()[1]<cachedValues[j + yres*i])
					v2 = -v2;
				total += v2;
				num++;

				v2 = Vector3D<float>(Point3D<float>(i+1,vscale*cachedValues[j + yres*(i+1)-1],j-1)-p1);
				v3 = Vector3D<float>(Point3D<float>(i+1,vscale*cachedValues[j + yres*(i+1)],j)-p1);
				v2 = v2.CrossProduct(v3).Normalize();
				if(v2.GetValue()[1]<cachedValues[j + yres*i])
					v2 = -v2;
				total += v2;
				num++;
			}
			total /= num;
			float* value = total.GetValue();
			normalValues[  (j + yres*i)*3] = value[0];
			normalValues[1+(j + yres*i)*3] = value[1];
			normalValues[2+(j + yres*i)*3] = value[2];
		}
		if(int(float(i)/float(xres)*100.0)%5==0)
			APP_LOG(SORE_Logging::LVL_DEBUG2,"%5.1f percent done", float(i)/float(xres)*100.0);
	}
	SORE_Resource::ResourceManager* rm = SORE_Resource::ResourceManager::GetManager();
	rm->RegisterLoader((SORE_Resource::RES_LOAD)SORE_Resource::LoadTexture, "tga");
	rm->Register("data/Textures/texture.tga");
	rm->Register("data/Textures/grass.tga");
	wireframe = false;
	normals = false;
	heightColor = false;
	perpixel = true;
	LightPosition[0] = (xres/2.0f)*scale;
	LightPosition[1] =  3.0f;
	LightPosition[2] = (yres/2.0f)*scale;
	lightMoveY = lightMoveX = lightMoveZ = 0.0f;
	
	const GLfloat LightAmbient[]=  {  0.4f, 0.4f, 0.4f, 1.0f };
	const GLfloat LightDiffuse[]=  {  0.8f, 0.8f, 0.8f, 1.0f };
	
	glEnable(GL_DEPTH_TEST); 
	
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.3);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	glColorMaterial ( GL_FRONT, GL_AMBIENT );
	glEnable(GL_COLOR_MATERIAL);
	glPolygonMode(GL_FRONT, GL_FILL);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	InitShaders();
	
	wireBox = glGenLists(1);
	glNewList(wireBox, GL_COMPILE);
	{
		glBegin(GL_LINE_LOOP);
				
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
	}
	glEndList();
	//glClearColor(0.7f,0.8f,0.9f,1.0f);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
}

bool SORE_Graphics::TerrainGraph::LightMoveCallback(SORE_Kernel::Event* event)
{
	switch(event->key.keySym)
	{
		case SDLK_LEFT:
			if(event->type == SORE_Kernel::KEYDOWN)
				lightMoveX = -1.0f;
			else
				lightMoveX = 0.0f;
			return true;
			break;
		case SDLK_RIGHT:
			if(event->type == SORE_Kernel::KEYDOWN)
				lightMoveX = 1.0f;
			else
				lightMoveX = 0.0f;
			return true;
			break;
		case SDLK_UP:
			if(event->type == SORE_Kernel::KEYDOWN)
				lightMoveY = 1.0f;
			else
				lightMoveY = 0.0f;
			return true;
			break;
		case SDLK_DOWN:
			if(event->type == SORE_Kernel::KEYDOWN)
				lightMoveY = -1.0f;
			else
				lightMoveY = 0.0f;
			return true;
		case SDLK_EQUALS:
			if(event->type == SORE_Kernel::KEYDOWN)
				lightMoveZ = 1.0f;
			else
				lightMoveZ = 0.0f;
			return true;
		case SDLK_MINUS:
			if(event->type == SORE_Kernel::KEYDOWN)
				lightMoveZ = -1.0f;
			else
				lightMoveZ = 0.0f;
			return true;
			break;
	}
	return false;
}

void SORE_Graphics::TerrainGraph::Frame(int elapsedTime)
{
	const float speed = 0.005;
	LightPosition[2] += lightMoveX*elapsedTime*speed;
	LightPosition[1] += lightMoveZ*elapsedTime*speed;
	LightPosition[0] += lightMoveY*elapsedTime*speed;
}

SORE_Graphics::TerrainGraph::~TerrainGraph()
{
	DestroyShaders();
	delete pn;
	delete cachedValues;
	delete normalValues;
}
	
void SORE_Graphics::TerrainGraph::Render()
{
	SORE_Resource::ResourceManager* rm = SORE_Resource::ResourceManager::GetManager();
	SORE_Resource::Resource* re;
	SORE_Resource::ResourceHandle* rd;
	
	const GLfloat lightPos[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(LightPosition[0], LightPosition[1], LightPosition[2]);
	if(!perpixel)
	{
		glDisable(GL_LIGHTING);
		glDisable( GL_COLOR_MATERIAL ) ;
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glCallList(wireBox);
	if(!perpixel)
	{
		glEnable(GL_LIGHTING);
		glEnable ( GL_COLOR_MATERIAL ) ;
	}
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glPopMatrix();
		
	re = rm->GetPtr("data/Textures/grass.tga");
	rd = dynamic_cast<SORE_Resource::ResourceHandle*>(re);
	if(perpixel)
	{
		glUseProgram(program);
	}
	else if(!heightColor && !perpixel)
	{
		glEnable(GL_TEXTURE_2D);
	}
	glBindTexture( GL_TEXTURE_2D, rd->GetHandle());
	for(int i=1;i<xres;i++)
	{
		if(wireframe)
			glBegin(GL_LINE_STRIP);
		else
			glBegin(GL_TRIANGLE_STRIP);
		float texY = i%2==1 ? 0.0 : 1.0;
		for(int j=0;j<yres;j++)
		{
			float texX;
			texX = j%2==0 ? 0.0 : 1.0;

			glTexCoord2f(texX, texY);
			glNormal3fv(&normalValues[(j + yres*i)*3]);
			if(heightColor)
				glColor4f(cachedValues[j + yres*i], cachedValues[j + yres*i], cachedValues[j + yres*i], 1.0f);
			glVertex3f(scale*i,vscale*cachedValues[j + yres*i], scale*j);
			
			glTexCoord2f(texX, texY-1.0);
			glNormal3fv(&normalValues[(j + yres*(i-1))*3]);
			if(heightColor)
				glColor4f(cachedValues[j + yres*(i-1)], cachedValues[j + yres*(i-1)], cachedValues[j + yres*(i-1)], 1.0f);
			glVertex3f(scale*i-scale,vscale*cachedValues[j + yres*(i-1)],scale*j);
		}
		glEnd();
	}

	if(perpixel)
		glUseProgram(0);
	else
		glDisable(GL_LIGHTING);
	if(normals)
	{
		for(int i=0;i<xres;i++)
		{
			for(int j=0;j<yres;j++)
			{
				glBegin(GL_LINES);
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(scale*i,vscale*cachedValues[j + yres*i], scale*j);
				glVertex3f(scale*(i+normalValues[(j + yres*i)*3]),(vscale*cachedValues[j + yres*i]+normalValues[(j + yres*i)*3+1]), scale*(j+normalValues[(j + yres*i)*3+2]));
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

void SORE_Graphics::TerrainGraph::InitShaders()
{
	char *frag, *vert;
	unsigned int fsize, vsize;
	SORE_FileIO::file_ref fptr, vptr;
	fptr = SORE_FileIO::Open("data/Shaders/fragment.shad");
	vptr = SORE_FileIO::Open("data/Shaders/vertex.shad");
	fsize = SORE_FileIO::Size(fptr);
	vsize = SORE_FileIO::Size(vptr);
	frag = new char[fsize+1];
	vert = new char[vsize+1];

	SORE_FileIO::Read(frag, sizeof(char), fsize, fptr);
	SORE_FileIO::Read(vert, sizeof(char), vsize, vptr);
	
	frag[fsize] = '\0';
	vert[vsize] = '\0';

	const char* frag_t = frag;
	const char* vert_t = vert;
	
	vertex   = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertex, 1, &vert_t,NULL);
	glShaderSource(fragment, 1, &frag_t, NULL);
	
	glCompileShader(vertex);
	glCompileShader(fragment);
	
	program = glCreateProgram();
	
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	
	
	glLinkProgram(program);
	glUseProgram(program);
	
	int success;
	int fras, vras;
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &vras);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &fras);
	
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	
	if(fras!=GL_TRUE || vras!=GL_TRUE)
	{
		APP_LOG_S(SORE_Logging::LVL_ERROR, "Failed to compile one or more shaders");
		
		int infologLength = 0;
		int charsWritten  = 0;
		char *infoLog;

		glGetShaderiv(fragment, GL_INFO_LOG_LENGTH,&infologLength);
		
		if (infologLength > 0)
		{
			infoLog = new char[infologLength];
			glGetShaderInfoLog(fragment, infologLength, &charsWritten, infoLog);
			APP_LOG(SORE_Logging::LVL_ERROR, "Failed to compile fragment program. Log follows.\n%s", infoLog);
			APP_LOG(SORE_Logging::LVL_ERROR, "Shader: \"%s\"", frag);
			delete[] infoLog;
		}
		else
			APP_LOG_S(SORE_Logging::LVL_INFO, "No info log for fragment shader");
		
		glGetShaderiv(vertex, GL_INFO_LOG_LENGTH,&infologLength);
		
		if (infologLength > 0)
		{
			infoLog = new char[infologLength];
			glGetShaderInfoLog(vertex, infologLength, &charsWritten, infoLog);
			APP_LOG(SORE_Logging::LVL_ERROR, "Failed to compile vertex program. Log follows.\n%s", infoLog);
			APP_LOG(SORE_Logging::LVL_ERROR, "Shader: \"%s\"", vert);
			delete[] infoLog;
		}
		else
			APP_LOG_S(SORE_Logging::LVL_INFO, "No info log for vertex shader");
	}
	else
	{
		APP_LOG_S(SORE_Logging::LVL_INFO, "Shaders compiled OK");
	}
	
	if(success!=GL_TRUE)
	{
		int infologLength = 0;
		int charsWritten  = 0;
		char *infoLog;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH,&infologLength);
		
		if (infologLength > 0)
		{
			infoLog = new char[infologLength];
			glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
			APP_LOG(SORE_Logging::LVL_ERROR, "Failed to link shader program. Log follows.\n%s", infoLog);
			delete[] infoLog;
		}
		else
			APP_LOG_S(SORE_Logging::LVL_ERROR, "Failed to link shader program. No info log.");
	}
	else
	{
		APP_LOG_S(SORE_Logging::LVL_INFO, "Shaders linked OK");
	}
	
	delete[] frag;
	delete[] vert;
	
	
	SORE_Resource::ResourceManager* rm = SORE_Resource::ResourceManager::GetManager();
	//rm->Register("data/Textures/texture.tga");
}

void SORE_Graphics::TerrainGraph::DestroyShaders()
{
	glDetachShader(program, fragment);
	glDetachShader(program, vertex);
	
	glDeleteShader(fragment);
	glDeleteShader(vertex);
	glDeleteProgram(program);
}
