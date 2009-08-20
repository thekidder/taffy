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

#ifndef SORE_VBO_H
#define SORE_VBO_H

#include <vector>
#include "sore_allgl.h"
#include "math/sore_matrix4x4.h"
namespace SORE_Graphics
{
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
    void AddObject(const GLfloat* v, const unsigned short* i, unsigned int numVertices, unsigned int numIndices, 
									 const SORE_Math::Matrix4<float>* transform = NULL, 
									 const GLfloat* t = NULL, const GLfloat* n = NULL, const GLfloat* c = NULL);
    void Build();
		
		void BeginDraw();
    void DrawAll();
		void DrawElements(unsigned int numTris, unsigned short triOffset);
		void EndDraw();
		unsigned int numIndices() const;
  private:
    GLuint vbo, vboIndices, vboNormals, vboTexCoords, vboColors;
		
    std::vector<GLfloat> vertices, normals, texCoords, colors;
    std::vector<unsigned short> indices;
  };
}
#endif
