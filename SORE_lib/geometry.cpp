
#include "geometry.h"

Triangle::Triangle()
{
}

Triangle::Triangle(Vertex* v)
{
	for(int i=0;i<3;i++)
	{
		vertices[i] = v[i];
	}
}

Triangle::Triangle(Vertex* v, const char* filename)
{
	for(int i=0;i<3;i++)
	{
		vertices[i] = v[i];
	}
}

Triangle::Triangle(Vertex* v, SDL_Surface* surface)
{
	for(int i=0;i<3;i++)
	{
		vertices[i] = v[i];
	}
}
