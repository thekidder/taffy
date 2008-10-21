/*
  Untitled Project
  Flatland-inspired RTS project code. Created by Adam Kidder.
  Licensing currently undecided; view as proprietary code.
*/
//$Id: geometrychunk.cpp 23 2008-10-08 02:14:59Z me $

#include <fstream>

#include "sore_util.h"
#include "sore_geometrychunk.h"

GeometryChunk::GeometryChunk() : vertices(0), texCoords(0), colors(0), indices(0), opaque(true), numVertices(0), numIndices(0)
{
}


GeometryChunk::~GeometryChunk()
{
  delete[] vertices;
  delete[] texCoords;
  delete[] colors;
  delete[] indices;
}

bool GeometryChunk::HasTexture() const
{
  return texCoords!=0;
}

bool GeometryChunk::HasColor() const
{
  return colors!=0;
}

/*void GeometryChunk::Load()
  {
  std::map<std::string, std::map<std::string, std::string> > list = SORE_Utility::ParseIniFile(filename.c_str());
		
  std::map<std::string, std::map<std::string, std::string> >::iterator i;
  std::map<std::string, std::string>::iterator i2;
		
  for(i=list.begin();i!=list.end();i++)
  {
  std::string section = i->first;
			
  for(i2=i->second.begin();i2!=i->second.end();i2++)
  {
  std::string name = i2->first;
  std::string value = i2->second;
			
  if(section == "Global")
  {
  if(name == "Material")
  {
  if(rm)
  mat = rm->GetResource<SORE_Graphics::Material>(value);
  else
  ENGINE_LOG(SORE_Logging::LVL_ERROR, "No resource manager set");
  }
  }
  else if(section == "Model")
  {
  if(name == "File")
  {
  LoadModel(value);
  }
  }
  }
  }
  }*/

const float * GeometryChunk::Vertices()
{
  return vertices;
}

const float * GeometryChunk::Colors()
{
  return colors;
}

const unsigned short * GeometryChunk::Indices()
{
  return indices;
}

const unsigned int GeometryChunk::NumVertices()
{
  return numVertices;
}

const unsigned int GeometryChunk::NumIndices()
{
  return numIndices;
}

/*void GeometryChunk::LoadModel(std::string filename)
  {
  std::ifstream model(filename.c_str());
	
  model >> numVertices;
  model >> numIndices;
	
  vertices = new float[numVertices*3];
  colors   = new float[numVertices*4];
  indices  = new unsigned short[numIndices];
	
  for(unsigned int i=0;i<numVertices;++i)
  {
  float x,y,z,r,g,b,a;
  model >> x;
  model >> y;
  model >> z;
  model >> r;
  model >> g;
  model >> b;
  model >> a;
		
  vertices[i*3 + 0] = x;
  vertices[i*3 + 1] = y;
  vertices[i*3 + 2] = z;
		
  colors[i*4 + 0] = r;
  colors[i*4 + 1] = g;
  colors[i*4 + 2] = b;
  colors[i*4 + 3] = a;
  }
	
  for(unsigned int i=0;i<numIndices;++i)
  {
  unsigned short temp;
  model >> temp;
  indices[i] = temp;
  }
  ENGINE_LOG(SORE_Logging::LVL_INFO, "Chunk parsed");
  }*/
