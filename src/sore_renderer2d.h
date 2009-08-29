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

#ifndef SORE_RENDERER2D_H
#define SORE_RENDERER2D_H

#include "sore_font.h"
#include "sore_geometrychunk.h"
#include "sore_renderer.h"
#include "sore_resource.h"
#include "sore_shaders.h"
#include "sore_timing.h"
#include "sore_fbo.h"
#include "sore_vbo.h"
#include "sore_graphics.h"
#include "sore_postprocess.h"

/**
   @author Adam Kidder <thekidder@gmail.com>
*/
namespace SORE_Graphics
{
	typedef std::pair<proj_callback, boost::function<render_list ()> > geometry_callback;

	struct vbo_tex_order
	{
		vbo_tex_order(const SORE_Resource::Texture2D* t, unsigned int s, unsigned int l)
			: tex(t), triStart(s), triLen(l) {}
		const SORE_Resource::Texture2D* tex;
		unsigned int triStart;
		unsigned int triLen;
	};

	class Renderer2D;

	typedef std::vector<vbo_tex_order> texture_stack;

	enum blend_mode {ADDITIVE, SUBTRACTIVE};

	struct geometry_provider
	{
		boost::function<render_list ()> geometryCallback;
		proj_callback projCallback;
		boost::function<const SORE_Math::Matrix4<float>& ()> cameraCallback;
		PostProcessEffect* effect;
		blend_mode blend;
	};

	struct batch
	{
		batch() {second = new VBO(true, false, true);}
		texture_stack first;
		VBO* second;
		PostProcessEffect* effect;
		GLenum blendSFactor, blendDFactor;
		proj_callback projCallback;
		boost::function<const SORE_Math::Matrix4<float>& ()> cameraCallback;
	};

	class Renderer2D : public SORE_Graphics::IRenderer
	{
	public:
		Renderer2D(SORE_Resource::ResourcePool& _rm);
		~Renderer2D();

		virtual void Render();

		void ClearGeometryProviders();
		void AddGeometryProvider(geometry_provider c);
		void Build();

		//Rendering Statistics
		float GetFPS() const;
		unsigned int GetDrawCalls() const;
		unsigned int GetNumPolys() const;

		void PushState();
		void PopState();		
	protected:
		void RenderBatch(batch& b);
		virtual void OnScreenChange();

		//returns 0 on success
		int ChangeProjectionMatrix(SORE_Graphics::ProjectionInfo& projection);
	private:
		void SetupProjection(SORE_Graphics::ProjectionInfo& pi);

		SORE_Resource::ResourcePool& rm;

		//acts as a stack: only providers on the top are called
		std::vector<
			std::vector<geometry_provider> 
			> geometryProviders;

		std::vector<std::vector<geometry_provider> >::iterator currentProvider;

		SORE_Graphics::GLSLShader* shad;
		std::vector<batch> batches;

		//statistics: get info about current rendering
		float fps;
		unsigned int drawCalls;
		unsigned int numPolys;
	};
				
	inline int TextureSort(const GeometryChunk* one, const GeometryChunk* two);
	bool GeometrySort(std::pair<const SORE_Math::Matrix4<float>*, const GeometryChunk*> one, std::pair<const SORE_Math::Matrix4<float>*, const GeometryChunk*> two);
}

#endif
