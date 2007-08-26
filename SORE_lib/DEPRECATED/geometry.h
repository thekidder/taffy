#ifndef  __GEOMETRY_H__
#define  __GEOMETRY_H__

#include "allgl.h"

struct Vertex
{
	GLfloat x, y, z; //geometry coordinates
	GLfloat u, v;    //texture coordinates
	GLfloat r, g, b; //texture colors
};

class Triangle
{
	public:
		Triangle();
		Triangle(Vertex* v);
		Triangle(Vertex* v, const char* filename);
		Triangle(Vertex* v, GLuint texture);
		void Draw();
		int SetTexture(const char* filename);
		~Triangle();
	protected:
		GLuint texture;
		Vertex vertices[3];
};

#endif //__GEOMETRY_H__
