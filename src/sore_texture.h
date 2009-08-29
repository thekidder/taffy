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
// $Id$

#ifndef SORE_TEXTURE_H
#define SORE_TEXTURE_H

#include "sore_resource.h"
#include "sore_allgl.h"

namespace SORE_Resource
{
  class SORE_EXPORT Texture2D : public Resource
  {
  public:
    Texture2D(std::string filename);
		Texture2D(const unsigned char* data, GLint internalFormat, 
							GLenum format, unsigned int width, unsigned int height);
    GLuint GetHandle() const;
    void Bind() const;
    const char* Type() const {return "2D texture";}

		void SaveTGA(const char* filename);
		bool GLContextDependent() const {return true;}
  protected:
    void Load();
    void LoadTGA(const char* filename);
		void LoadFromData(const unsigned char* data, GLint internalFormat, 
											GLenum format, unsigned int width, unsigned int height);
    void Unload();
    virtual std::string ProcessFilename(std::string file) {return file;}
    GLuint handle;
	private:
		unsigned int w, h;
  };
}

#endif
