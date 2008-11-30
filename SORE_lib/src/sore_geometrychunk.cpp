/*
  Untitled Project
  Flatland-inspired RTS project code. Created by Adam Kidder.
  Licensing currently undecided; view as proprietary code.
*/
//$Id$

#include <fstream>

#include "sore_util.h"
#include "sore_geometrychunk.h"

namespace SORE_Graphics
{
	GeometryChunk::GeometryChunk(SORE_Resource::Texture2D* texture, SORE_Math::Rect<float> bounds) : colors(0), opaque(true)
	{
		setup(texture, bounds.topLeft[0], bounds.bottomRight[0], bounds.topLeft[1], bounds.bottomRight[1]);
	}

	GeometryChunk::GeometryChunk(SORE_Resource::Texture2D* texture, float left, float right, float top, float bottom) :
		colors(0), opaque(true)
	{
		setup(texture, left, right, top, bottom);
	}

	void GeometryChunk::setup(SORE_Resource::Texture2D* texture, float left, float right, float top, float bottom)
	{
		vertices = new float[12];
		texCoords = new float[8];
		indices = new unsigned short[6];
		vertices[0] = left;
		vertices[1] = top;
		vertices[2] = 0.0;

		vertices[3] = left;
		vertices[4] = bottom;
		vertices[5] = 0.0;

		vertices[6] = right;
		vertices[7] = top;
		vertices[8] = 0.0;

		vertices[9] = right;
		vertices[10] = bottom;
		vertices[11] = 0.0;

		texCoords[0] = 0.0;
		texCoords[1] = 1.0;

		texCoords[2] = 0.0;
		texCoords[3] = 0.0;

		texCoords[4] = 1.0;
		texCoords[5] = 1.0;

		texCoords[6] = 1.0;
		texCoords[7] = 0.0;

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 1;
		indices[5] = 3;

		numVertices = 4;
		numIndices = 6;
		tex = texture;
	}

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

	const float * GeometryChunk::Vertices() const
	{
		return vertices;
	}

	const float * GeometryChunk::Colors() const
	{
		return colors;
	}

	const unsigned short * GeometryChunk::Indices() const
	{
		return indices;
	}

	const float* GeometryChunk::TexCoords() const
	{
		return texCoords;
	}

	unsigned int GeometryChunk::NumVertices() const
	{
		return numVertices;
	}

	unsigned int GeometryChunk::NumIndices() const
	{
		return numIndices;
	}

	const SORE_Resource::Texture2D* GeometryChunk::GetTexture() const
	{
		return tex;
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
	inline int NullGeometrySort(SORE_Graphics::GeometryChunk* one, SORE_Graphics::GeometryChunk* two)
	{
		if(one < two) return SORT_LESS;
		else if(one == two) return SORT_EQUAL;
		else return SORT_GREATER; 
	}
}
