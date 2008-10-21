/*
Untitled Project
Flatland-inspired RTS project code. Created by Adam Kidder.
Licensing currently undecided; view as proprietary code.
*/
//$Id: vbo.h 23 2008-10-08 02:14:59Z me $

#ifndef DYNAMICVBO_H
#define DYNAMICVBO_H

#include <vector>
#include "sore_allgl.h"
#include "math/sore_matrix4x4.h"

/**
	@author Adam Kidder <thekidder@gmail.com>
	For use with dynamic (change every frame) indexed VBOs
*/
class VBO
{
	public:
		VBO(bool t = false, bool n = false, bool c = false);
		~VBO();
		
		void Clear();
		void AddObject(const GLfloat* v, const unsigned short* i, unsigned int numVertices, unsigned int numIndices, const SORE_Math::Matrix4<float>* transform = NULL, const GLfloat* t = NULL, const GLfloat* n = NULL, const GLfloat* c = NULL);
		void Build();
		
		void Render();
	private:
		GLuint vbo, vboIndices, vboNormals, vboTexCoords, vboColors;
		
		std::vector<GLfloat> vertices, normals, texCoords, colors;
		std::vector<unsigned short> indices;
};

#endif
