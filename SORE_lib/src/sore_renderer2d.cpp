/*
  Untitled Project
  Flatland-inspired RTS project code. Created by Adam Kidder.
  Licensing currently undecided; view as proprietary code.
*/
//$Id$

#include "sore_renderer2d.h"

#include "sore_allgl.h"
#include "sore_shaders.h"
#include "sore_graphics.h"

#include <algorithm>

namespace SORE_Graphics
{

	Renderer2D::Renderer2D(SORE_Resource::ResourcePool& _rm) : rm(_rm), all(true)
	{
		font = rm.GetResource<SORE_Font::Font>("arial.ttf", "32");
		shad = rm.GetResource<SORE_Graphics::GLSLShader>("data/Shaders/default.shad");
	}

	Renderer2D::~Renderer2D()
	{
	}

	void Renderer2D::OnProjectionChange()
	{
	}

	void Renderer2D::OnScreenChange()
	{
	}

	void Renderer2D::Build()
	{
		if(!geometryCallback)
			return;
	
		all.Clear();
		textureStack.clear();
		render_list geometry = geometryCallback();
		if(!geometry.size())
			return;
		std::sort(geometry.begin(), geometry.end(), &GeometrySort);
		render_list::iterator it;
		unsigned int numIndices = 0, totalIndices = 0;
		//ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Beginning geometry build"));
		for(it=geometry.begin();it!=geometry.end();++it)
		{
			all.AddObject(it->second->Vertices(), it->second->Indices(), it->second->NumVertices(), 
										it->second->NumIndices(), it->first, it->second->TexCoords(), NULL, it->second->Colors());
			if(!textureStack.size() || textureStack.back().tex->GetHandle() != it->second->GetTexture()->GetHandle())
			{
				if(textureStack.size()>0)
				{
					//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Modifying len of previous entry: %d") % (numIndices/3));
					textureStack.back().triLen = numIndices/3;
				}
				//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Adding entry to textureStack: start %d tex %d")
				//					 % (numIndices/3) % (it->second->GetTexture()->GetHandle()));
				textureStack.push_back(
					vbo_tex_order(it->second->GetTexture(), totalIndices/3, 0));
				numIndices = 0;
			}
			numIndices+=it->second->NumIndices();
			totalIndices+=it->second->NumIndices();
		}
		//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Modifying len of previous entry: %d") % (totalIndices/3 - textureStack.back().triStart));
		textureStack.back().triLen = totalIndices/3 - textureStack.back().triStart;

		all.Build();
	}

	void Renderer2D::Render()
	{
		glEnable(GL_BLEND);
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		glLoadIdentity();
		if(camera)
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(const_cast<const GLfloat*>(camera().GetData()));
		}
	
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Build();
		if(all.numIndices())
		{
			shad->Bind();
			textureStack.begin()->tex->Bind();
			all.BeginDraw();
			for(std::vector<vbo_tex_order>::iterator it=textureStack.begin();it!=textureStack.end();++it)
			{
				it->tex->Bind();
				all.DrawElements(it->triLen, it->triStart);
			}
			all.EndDraw();
		}
		static int frames = 0;
		static int T0 = SORE_Timing::GetGlobalTicks();
		static float fps;
		frames++;
		{
			GLint t = SORE_Timing::GetGlobalTicks();
			if (t - T0 >= 500) //calculate FPS every 50 milliseconds
			{
				GLfloat seconds = (GLfloat)((t - T0) / 10000.0);
				fps = frames / seconds;
				T0 = t;
				frames = 0;
			}
		}
		
		SORE_Graphics::GLSLShader::UnbindShaders();

		SORE_Graphics::PushOverlay();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		if(font)
		{
			font->Print(SORE_Graphics::GetWidth()-140, 0,  boost::format("FPS: %4.0f") % fps);
		}
		SORE_Graphics::PopOverlay();
	}

	void Renderer2D::SetGeometryCallback(boost::function<render_list ()> c)
	{
		geometryCallback = c;
	}

	void Renderer2D::SetCameraCallback(boost::function<const SORE_Math::Matrix4 < float > & ( ) > c)
	{
		camera = c;
	}

	inline int TextureSort(const GeometryChunk* one, const GeometryChunk* two)
	{
		if(one->GetTexture()->GetHandle() < two->GetTexture()->GetHandle()) return SORT_LESS;
		else if(one->GetTexture() == two->GetTexture()) return SORT_EQUAL;
		else return SORT_GREATER;
	}

	bool GeometrySort(std::pair<const SORE_Math::Matrix4<float>*, const GeometryChunk*> one, std::pair<const SORE_Math::Matrix4<float>*, const GeometryChunk*> two)
	{
		if(TextureSort(one.second, two.second) == SORT_LESS) return true;
		return false;
	}
}
