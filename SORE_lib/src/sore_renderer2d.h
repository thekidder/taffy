/*
  Untitled Project
  Flatland-inspired RTS project code. Created by Adam Kidder.
  Licensing currently undecided; view as proprietary code.
*/
//$Id: dimensionalityrenderer.h 23 2008-10-08 02:14:59Z me $

#ifndef DIMENSIONALITYRENDERER_H
#define DIMENSIONALITYRENDERER_H

#include "sore_renderer.h"
#include "sore_resource.h"
#include "sore_geometrychunk.h"
#include "sore_shaders.h"
#include "sore_vbo.h"

/**
   @author Adam Kidder <thekidder@gmail.com>
*/
namespace SORE_Graphics
{
	struct vbo_tex_order
	{
	vbo_tex_order(const SORE_Resource::Texture2D* t, unsigned int s, unsigned int l) : tex(t), triStart(s), triLen(l) {}
		const SORE_Resource::Texture2D* tex;
		unsigned int triStart;
		unsigned int triLen;
	};

				class Renderer2D : public SORE_Graphics::IRenderer
				{
				public:
								Renderer2D(SORE_Resource::ResourcePool& _rm);
								~Renderer2D();

								virtual void Render();

								void SetGeometryCallback(boost::function<render_list ()> c);
								void SetCameraCallback(boost::function<SORE_Math::Matrix4<float>& ()> c);
								void Build();
		
				protected:
								virtual void OnProjectionChange();
								virtual void OnScreenChange();
				private:
								SORE_Resource::ResourcePool& rm;
								GLuint fbo, depthbuffer, color0, color1;

								boost::function<render_list ()> geometryCallback;
								boost::function<SORE_Math::Matrix4<float>& ()> camera;
		
								SORE_Font::Font* font;
								SORE_Graphics::GLSLShader* shad;
								//std::vector<VBO> batches;
								VBO all;
								std::vector<vbo_tex_order > textureStack; 
				};
				
				inline int TextureSort(const GeometryChunk* one, const GeometryChunk* two);
				bool GeometrySort(std::pair<const SORE_Math::Matrix4<float>*, const GeometryChunk*> one, std::pair<const SORE_Math::Matrix4<float>*, const GeometryChunk*> two);
}
#endif
