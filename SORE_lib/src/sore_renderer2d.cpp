/*
  Untitled Project
  Flatland-inspired RTS project code. Created by Adam Kidder.
  Licensing currently undecided; view as proprietary code.
*/
//$Id: dimensionalityrenderer.cpp 23 2008-10-08 02:14:59Z me $

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
								std::sort(geometry.begin(), geometry.end());
								render_list::iterator it;
								for(it=geometry.begin();it!=geometry.end();++it)
								{
												all.AddObject(it->second->Vertices(), it->second->Indices(), it->second->NumVertices(), it->second->NumIndices(), it->first, it->second->TexCoords(), NULL, it->second->Colors());
												if(!textureStack.size() || (--textureStack.end())->second != it->second->GetTexture())
																textureStack.push_back(std::make_pair(it - geometry.begin(), it->second->GetTexture()));
								}

								ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("texture starting index %d") % textureStack[0].first);

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
	
								//Build();

								shad->Bind();
								glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
								textureStack.begin()->second->Bind();
								all.BeginDraw();
								all.DrawElements(2, 0);
								all.DrawElements(2, 2);
								all.DrawAll();
								all.EndDraw();

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

				void Renderer2D::SetCameraCallback(boost::function< SORE_Math::Matrix4 < float > & ( ) > c)
				{
								camera = c;
				}

				inline int TextureSort(GeometryChunk* one, GeometryChunk* two)
				{
								if(one->GetTexture() < two->GetTexture()) return SORT_LESS;
								else if(one->GetTexture() == two->GetTexture()) return SORT_EQUAL;
								else return SORT_GREATER;
				}

				bool operator<(std::pair<SORE_Math::Matrix4<float>, GeometryChunk*> one, std::pair<SORE_Math::Matrix4<float>, GeometryChunk*> two)
				{
								if(TextureSort(one.second, two.second) == SORT_LESS) return true;
								return false;
				}
}
