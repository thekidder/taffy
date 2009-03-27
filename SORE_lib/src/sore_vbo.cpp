/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//$Id$

#include "sore_vbo.h"
#include "sore_logger.h"
#include <algorithm>

namespace SORE_Graphics
{
	VBO::VBO(bool t, bool n, bool c) : vbo(0), vboIndices(0), vboNormals(0), vboTexCoords(0), vboColors(0)
	{
		glGenBuffersARB(1, &vbo);
		if(!vbo)
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create vertex buffer");
		glGenBuffersARB(1, &vboIndices);
		if(!vboIndices)
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create index buffer");
		if(t)
		{
			glGenBuffersARB(1, &vboTexCoords);
			if(!vboTexCoords)
				ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create texture coordinate buffer");
		}
		if(n)
		{
			glGenBuffersARB(1, &vboNormals);
			if(!vboNormals)
				ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create normal buffer");
		}
		if(c)
		{
			glGenBuffersARB(1, &vboColors);
			if(!vboColors)
				ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not create color buffer");
		}
	}


	VBO::~VBO()
	{
		glDeleteBuffersARB(1, &vbo);
		glDeleteBuffersARB(1, &vboIndices);
		if(vboColors)
			glDeleteBuffersARB(1, &vboColors);
		if(vboNormals)
			glDeleteBuffersARB(1, &vboNormals);
		if(vboTexCoords)
			glDeleteBuffersARB(1, &vboTexCoords);
	}

	void VBO::AddObject(const GLfloat* v, const unsigned short* i, unsigned int numVertices, 
											unsigned int numIndices, const SORE_Math::Matrix4<float>* transform, 
											const GLfloat* t, const GLfloat* n, const GLfloat* c)
	{
		if( (vboNormals && !n) || (vboTexCoords && !t) || (vboColors && !c) )
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Invalid VBO format; check constructer");
		}
	
		std::insert_iterator<std::vector<GLfloat> > ii(vertices, vertices.end());
	
		size_t oldSize = vertices.size();
		if(!transform)
		{
			std::copy(v, v+numVertices*3, ii);
		}
		else
		{
			for(unsigned int i=0;i<numVertices;++i)
			{
				SORE_Math::Vector4<float> pos(v[i*3], v[i*3+1],v[i*3+2], 1.0f);

				pos = *transform * pos;
				vertices.push_back(pos[0]);
				vertices.push_back(pos[1]);
				vertices.push_back(pos[2]);
			}
		}
	
		if(vboColors)
		{
			ii = std::insert_iterator<std::vector<GLfloat> >(colors, colors.end());
			std::copy(c, c+numVertices*4, ii);
		}

		if(vboTexCoords)
		{
			ii = std::insert_iterator<std::vector<GLfloat> >(texCoords, texCoords.end());
			std::copy(t, t+numVertices*2, ii);
		}

		//copy vertices into VBO, taking into account index renumbering
		for(unsigned short j=0;j<numIndices;++j)
		{
			unsigned short index = *(i + j) + oldSize/3;
			indices.push_back(index);
		}
	}

	void VBO::BeginDraw()
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		if(vboTexCoords)
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		if(vboNormals)
			glEnableClientState(GL_NORMAL_ARRAY);
		if(vboColors)
			glEnableClientState(GL_COLOR_ARRAY);
	
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vboIndices);
		glVertexPointer(3, GL_FLOAT, 0, 0);
	
		if(vboNormals)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboNormals);
			glNormalPointer(GL_FLOAT, 0, 0);
		}
		if(vboTexCoords)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboTexCoords);
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
		}
		if(vboColors)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboColors);
			glColorPointer(4, GL_FLOAT, 0, 0);
		}
	}

	void VBO::EndDraw()
	{
		if(vboColors)
			glDisableClientState(GL_COLOR_ARRAY);
		if(vboNormals)
			glDisableClientState(GL_NORMAL_ARRAY);
		if(vboTexCoords)
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	void VBO::DrawAll()
	{
		glDrawRangeElements(GL_TRIANGLES, 0, indices.size(), indices.size(), GL_UNSIGNED_SHORT, 0);
	}

	void VBO::DrawElements(unsigned int numTris, unsigned short triOffset)
	{
		glDrawRangeElements(GL_TRIANGLES, 0, indices.size(), numTris*3, GL_UNSIGNED_SHORT, (char*)NULL + (triOffset*3)*sizeof(unsigned short));
	}

	void VBO::Build()
	{
		if(!indices.size()) return;
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vboIndices);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indices.size()*sizeof(unsigned short), &indices[0], GL_DYNAMIC_DRAW_ARB);
	
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertices.size()*sizeof(GLfloat), &vertices[0], GL_DYNAMIC_DRAW_ARB);
	
		if(vboColors)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboColors);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, colors.size()*sizeof(GLfloat), &colors[0], GL_DYNAMIC_DRAW_ARB);
		}

		if(vboTexCoords)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboTexCoords);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, texCoords.size()*sizeof(GLfloat), &texCoords[0], GL_DYNAMIC_DRAW_ARB);
		}
	}

	void VBO::Clear()
	{
		vertices.clear();
		normals.clear();
		texCoords.clear();
		colors.clear();
		indices.clear();
	}

	unsigned int VBO::numIndices() const
	{
		return indices.size();
	}
}
