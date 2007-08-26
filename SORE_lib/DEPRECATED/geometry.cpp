
#include "geometry.h"
#include "texture.h"

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
	switch(SORE_Texture::ImageFormat(filename))
	{
		case SORE_Texture::IMG_TGA:
			texture = SORE_Texture::LoadTGA(filename);
			break;
		default:
			std::cerr << "Could not determine texture format\n";
	}
}

Triangle::Triangle(Vertex* v, GLuint texture)
{
	for(int i=0;i<3;i++)
	{
		vertices[i] = v[i];
	}
	this->texture = texture;
}
