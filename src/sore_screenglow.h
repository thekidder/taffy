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

#ifndef SORE_SCREENGLOW_H
#define SORE_SCREENGLOW_H

#include "sore_fbo.h"
#include "sore_resource.h"
#include "sore_shaders.h"
#include "sore_postprocess.h"

namespace SORE_Graphics
{
	class SORE_EXPORT ScreenGlow : public PostProcessEffect
	{
	public:
		ScreenGlow(unsigned int w, unsigned int h, SORE_Resource::ResourcePool& pool);
		~ScreenGlow();

		void StartFrame(ProjectionInfo& proj);
		void EndFrame(ProjectionInfo& proj);

	private:
		void OnScreenChange();

		FBO* fbo;

		GLSLShader* blur, *combine;
	};
}

#endif
