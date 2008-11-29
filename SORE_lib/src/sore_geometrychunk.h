/*
Untitled Project
Flatland-inspired RTS project code. Created by Adam Kidder.
Licensing currently undecided; view as proprietary code.
*/
//$Id: geometrychunk.h 23 2008-10-08 02:14:59Z me $

#ifndef GEOMETRYCHUNK_H
#define GEOMETRYCHUNK_H

#include <boost/function.hpp>

#include <map>
#include "sore_material.h"
#include "math/sore_matrix4x4.h"

/**
   @author Adam Kidder <thekidder@gmail.com>
*/
namespace SORE_Graphics
{
  class GeometryChunk
  {
  public:
    GeometryChunk(SORE_Resource::Texture2D* texture, float left, float right, float top, float bottom); //create sprite
    GeometryChunk();
    ~GeometryChunk();
		
    bool HasTexture() const;
    bool HasColor() const;
		
    const float* Vertices() const;
    const float* Colors() const;
		const float* TexCoords() const;
    const unsigned short* Indices() const;
		
    unsigned int NumVertices() const;
    unsigned int NumIndices() const;
		
		const SORE_Resource::Texture2D* GetTexture() const;

  protected:
  private:
    //geometry
    float* vertices;
    float* texCoords;
    float* colors;
		
    unsigned short* indices;
    bool opaque;
		
    unsigned int numVertices, numIndices;
		
    SORE_Resource::Texture2D* tex;
		
    unsigned int primitiveType;
  };

  typedef std::vector<std::pair<const SORE_Math::Matrix4<float>*, const GeometryChunk *> > render_list;

	const int SORT_LESS    = -1;
	const int SORT_EQUAL   =  0;
	const int SORT_GREATER =  1;

	//typedef boost::function<int (GeometryChunk*, GeometryChunk*)> geometry_sort;

	//inline int NullGeometrySort(SORE_Graphics::GeometryChunk* one, SORE_Graphics::GeometryChunk* two); //sorts by ptr
}
#endif
