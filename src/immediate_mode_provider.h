#ifndef IMMEDIATE_MODE_PROVIDER_H
#define IMMEDIATE_MODE_PROVIDER_H

#include <sore_geometryprovider.h>
#include <sore_glslshader.h>
#include <sore_simplebuffermanager.h>
#include <sore_texture.h>

#include <boost/unordered_map.hpp>

class ImmediateModeProvider : public SORE_Graphics::GeometryProvider
{
public:
    ImmediateModeProvider(SORE_Graphics::Texture2DPtr default_texture, SORE_Graphics::GLSLShaderPtr default_shader);

    void SetTexture(SORE_Graphics::Texture2DPtr texture);
    void SetShader(SORE_Graphics::GLSLShaderPtr shader);
    void SetColor(SORE_Graphics::Color color);
    void SetBlendMode(SORE_Graphics::blend_mode blend_mode);
    void SetPrimitiveType(GLenum type);

    void Start();
    void DrawQuad(
        float x1, float y1, float z1,
        float x2, float y2, float z2,
        float x3, float y3, float z3,
        float x4, float y4, float z4);
    void DrawLine(
        float x1, float y1, float z1,
        float x2, float y2, float z2);

    // geometry provider interface
    void MakeUpToDate();
    std::vector<SORE_Graphics::Renderable>::iterator GeometryBegin();
    std::vector<SORE_Graphics::Renderable>::iterator GeometryEnd();

    // buffer manager interface
    void Regenerate();

    SORE_Graphics::BufferManager* GetBufferManager() { return &buffer_manager; }
private:
    void SetTexCoords(float i, float j);
    void AddVertex(float x, float y, float z);

    // flush the current vertex/index cache to a new renderable and start a new cache
    void CreateRenderableFromData();

    SORE_Graphics::Texture2DPtr current_texture;
    SORE_Graphics::GLSLShaderPtr current_shader;
    SORE_Graphics::Color current_color;
    SORE_Graphics::blend_mode current_blend_mode;
    GLenum current_primitive_type;
    std::pair<float, float> current_texcoords;

    std::vector<SORE_Graphics::vertex> vertices;
    std::vector<unsigned short> indices;

    SORE_Graphics::SimpleBufferManager buffer_manager;
    std::vector<SORE_Graphics::Renderable> renderables;
};

#endif