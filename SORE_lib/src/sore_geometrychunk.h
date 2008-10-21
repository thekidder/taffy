/*
Untitled Project
Flatland-inspired RTS project code. Created by Adam Kidder.
Licensing currently undecided; view as proprietary code.
*/
//$Id: geometrychunk.h 23 2008-10-08 02:14:59Z me $

#ifndef GEOMETRYCHUNK_H
#define GEOMETRYCHUNK_H

#include <map>
#include "sore_material.h"
#include "math/sore_matrix4x4.h"

/**
   @author Adam Kidder <thekidder@gmail.com>
*/
class GeometryChunk
{
 public:
  GeometryChunk();
  ~GeometryChunk();
		
  bool HasTexture() const;
  bool HasColor() const;
		
  const float* Vertices();
  const float* Colors();
  const unsigned short* Indices();
		
  const unsigned int NumVertices();
  const unsigned int NumIndices();
				
 protected:
 private:
  //geometry
  float* vertices;
  float* texCoords;
  float* colors;
		
  unsigned short* indices;
  bool opaque;
		
  unsigned int numVertices, numIndices;
		
  SORE_Graphics::Material* mat;
		
  unsigned int primitiveType;
};

typedef std::map<const SORE_Math::Matrix4<float>*, GeometryChunk * > render_list;

#endif
