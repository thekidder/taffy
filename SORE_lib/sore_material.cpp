/***************************************************************************
 *   Copyright (C) 2008 by Adam Kidder                                     *
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

#include "sore_material.h"
#include "sore_util.h"

namespace SORE_Graphics
{
	Material::Material(const char* materialFile) : shader(NULL)
	{
		LoadFromFile(materialFile);
	}
	
	Material::~Material()
	{
		if(shader) delete shader;
	}
	
	void Material::LoadFromFile(const char* materialFile)
	{
		std::map<std::string, std::map<std::string, std::string> > list = SORE_Utility::ParseIniFile(materialFile);
		
		std::map<std::string, std::map<std::string, std::string> >::iterator i;
		std::map<std::string, std::string>::iterator i2;
		
		for(i=list.begin();i!=list.end();i++)
		{
			std::string section = i->first;
			
			for(i2=i->second.begin();i2!=i->second.end();i2++)
			{
				std::string name = i2->first;
				std::string value = i2->second;
				
				if(section=="Textures")
				{
				}
				else if(section=="Shader")
				{
					if(name=="shader_file")
					{
						shader = new GLSLShader(value.c_str());
					}
				}
				else if(section=="Fixed-Function")
				{
				}
				else if(section=="Texture-Combine")
				{
				}
				else
				{
					ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Invalid material heading: %s") % section);
				}
			}
		}
	}
}
