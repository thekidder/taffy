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

#ifndef  __SORE_RENDERER__
#define  __SORE_RENDERER__

#include "sore_2dprimitives.h"
#include "sore_font.h"
#include "sore_timing.h"

namespace SORE_Graphics
{
	typedef unsigned int gc_id;
	
	class IRenderer
	{
		public:
			IRenderer() {}
			virtual ~IRenderer() {}
			
			virtual void Render() {}
	};
	
	class Renderer2D : public IRenderer
	{
		public:
			Renderer2D();
			~Renderer2D();
			
			gc_id AddRenderable(IRenderable gc);
			IRenderable* GeometryChunkPtr(gc_id id);
			void RemoveGeometryChunk(gc_id gc);
			
			void Render();
		protected:
			std::map<gc_id, IRenderable> geometry;
			std::list<gc_id> unusedIds;
			SORE_Font::font_ref font;
	};
}

#endif /*__SORE_RENDERER__*/
