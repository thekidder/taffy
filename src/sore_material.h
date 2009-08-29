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

#ifndef  SORE_MATERIAL_H
#define  SORE_MATERIAL_H

#include "sore_resource.h"
#include "sore_shaders.h"
#include "sore_texture.h"

#include <map>
#include <vector>

namespace SORE_Graphics
{
  class Material : public SORE_Resource::Resource
    {
    public:
      Material(std::string materialFile, std::string additionalInfo);
      ~Material();
			
      void Bind();
			
      std::vector<std::string> GetFlags();
      void                     AddFlag(std::string flag);
      bool                     HasFlag(std::string flag);
    protected:
      void Load();
      GLSLShader* shader;
      bool useShader;
      virtual std::string ProcessFilename(std::string file) {return file;}
      std::string file;
      //std::vector<std::pair<std::string, std::pair<int, SORE_Resource::Texture2D*> > > textures;
      std::map<std::string, std::pair<int, SORE_Resource::Texture2D*> > textureMap;
      //std::vector<std::map<std::string, std::pair<int, SORE_Resource::Texture2D*> >::iterator> textureOrder;
      std::vector<std::string> textureOrder;
      std::vector<std::string> flags;
    };
}

#endif
