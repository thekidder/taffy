/***************************************************************************
 *   Copyright (C) 2007-2008 by Adam Kidder                                *
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
// $Id$

#ifndef  __SORE_MATERIAL__
#define  __SORE_MATERIAL__

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
			Material(std::string materialFile);
			~Material();
			
			void Bind();
		protected:
			void Load();
			GLSLShader* shader;
			bool useShader;
			std::string file;
			//std::vector<std::pair<std::string, std::pair<int, SORE_Resource::Texture2D*> > > textures;
			std::map<std::string, std::pair<int, SORE_Resource::Texture2D*> > textureMap;
			//std::vector<std::map<std::string, std::pair<int, SORE_Resource::Texture2D*> >::iterator> textureOrder;
			std::vector<std::string> textureOrder;
	};
}

#endif /*__SORE_MATERIAL__*/
