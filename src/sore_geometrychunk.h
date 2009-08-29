/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//$Id$

#ifndef SORE_GEOMETRYCHUNK_H
#define SORE_GEOMETRYCHUNK_H

#include <boost/function.hpp>

#include <map>
#include "sore_color.h"
#include "sore_texture.h"
#include "math/sore_matrix4x4.h"
#include "math/sore_geometry.h"

/**
   @author Adam Kidder <thekidder@gmail.com>
*/
namespace SORE_Graphics
{
	static SORE_Math::Rect<float> defaultTexCoords(0.0f, 1.0f, 0.0f, 1.0f);

  class SORE_EXPORT GeometryChunk
  {
  public:
		GeometryChunk(SORE_Resource::Texture2D* texture, SORE_Math::Rect<float> bounds,
									SORE_Math::Rect<float> texCoords = defaultTexCoords,
									const Color& color = White); 
    GeometryChunk();
    ~GeometryChunk();
		
		const Color& GetColor() const;
		void SetColor(const Color& color);

    bool HasTexture() const;
    bool HasColor() const;
		bool IsOpaque() const;		
    const float* Vertices() const;
    const float* Colors() const;
		const float* TexCoords() const;
    const unsigned short* Indices() const;
		
    unsigned int NumVertices() const;
    unsigned int NumIndices() const;
		
		const SORE_Resource::Texture2D* GetTexture() const;
		void SetTexture(SORE_Resource::Texture2D* texture);

  protected:
  private:
		void setup(SORE_Math::Rect<float> bounds,
							 SORE_Math::Rect<float> texCoordRect);

    //geometry
    float* vertices;
    float* texCoords;
    float* colors;
		
    unsigned short* indices;
    bool opaque;
		
    unsigned int numVertices, numIndices;
		
    SORE_Resource::Texture2D* tex;
		Color c;
    unsigned int primitiveType;
  };

  typedef std::vector<std::pair<const SORE_Math::Matrix4<float>*, const GeometryChunk *> > 
		render_list;

  typedef std::vector<std::pair<SORE_Math::Matrix4<float>, const GeometryChunk *> > 
		render_list_owned;

	void ApplyTransform(const SORE_Math::Matrix4<float>& transform, render_list_owned& list);
	render_list GetRenderList(render_list_owned& list);

	const int SORT_LESS    = -1;
	const int SORT_EQUAL   =  0;
	const int SORT_GREATER =  1;
}
#endif
