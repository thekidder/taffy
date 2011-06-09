#include "immediate_mode_provider.h"

ImmediateModeProvider::ImmediateModeProvider(SORE_Graphics::Texture2DPtr default_texture, SORE_Graphics::GLSLShaderPtr default_shader)
    : current_texture(default_texture), current_shader(default_shader), current_blend_mode(SORE_Graphics::BLEND_ADDITIVE), current_primitive_type(GL_TRIANGLES)
{
}

void ImmediateModeProvider::SetTexture(SORE_Graphics::Texture2DPtr texture)
{
    CreateRenderableFromData();
    current_texture = texture;
}

void ImmediateModeProvider::SetShader(SORE_Graphics::GLSLShaderPtr shader)
{
    CreateRenderableFromData();
    current_shader = shader;
}

void ImmediateModeProvider::SetColor(SORE_Graphics::Color color)
{
    current_color = color;
}

void ImmediateModeProvider::SetBlendMode(SORE_Graphics::blend_mode blend_mode)
{
    CreateRenderableFromData();
    current_blend_mode = blend_mode;
}

void ImmediateModeProvider::SetPrimitiveType(GLenum type)
{
    CreateRenderableFromData();
    current_primitive_type = type;
}

void ImmediateModeProvider::Start()
{
    buffer_manager.Clear();
    renderables.clear();
    vertices.clear();
    indices.clear();
}

void ImmediateModeProvider::DrawQuad(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4)
{
    if(current_primitive_type != GL_TRIANGLES)
        SetPrimitiveType(GL_TRIANGLES);

    if(indices.size() + 6 >= 65535)
        CreateRenderableFromData();

    unsigned short current_index = vertices.size();

    SetTexCoords(0.0f, 0.0f);
    AddVertex(x1, y1, z1);

    SetTexCoords(0.0f, 1.0f);
    AddVertex(x2, y2, z2);

    SetTexCoords(1.0f, 0.0f);    
    AddVertex(x3, y3, z3);

    SetTexCoords(1.0f, 1.0f);
    AddVertex(x4, y4, z4);

    // add two triangles
    indices.push_back(current_index);
    indices.push_back(current_index + 1);
    indices.push_back(current_index + 2);
    indices.push_back(current_index + 2);
    indices.push_back(current_index + 1);
    indices.push_back(current_index + 3);
}

void ImmediateModeProvider::DrawLine(
    float x1, float y1, float z1,
    float x2, float y2, float z2)
{
    if(current_primitive_type != GL_LINES)
        SetPrimitiveType(GL_LINES);

    if(indices.size() + 6 >= 65535)
        CreateRenderableFromData();

    unsigned short current_index = vertices.size();

    SetTexCoords(0.0f, 0.0f);
    AddVertex(x1, y1, z1);

    SetTexCoords(0.0f, 1.0f);
    AddVertex(x2, y2, z2);

    indices.push_back(current_index);
    indices.push_back(current_index + 1);
}

void ImmediateModeProvider::MakeUpToDate()
{
    CreateRenderableFromData();
    buffer_manager.MakeUpToDate();
}

std::vector<SORE_Graphics::Renderable>::iterator ImmediateModeProvider::GeometryBegin()
{
    return renderables.begin();
}

std::vector<SORE_Graphics::Renderable>::iterator ImmediateModeProvider::GeometryEnd()
{
    return renderables.end();
}

void ImmediateModeProvider::Regenerate()
{
    buffer_manager.Regenerate();
}

void ImmediateModeProvider::SetTexCoords(float i, float j)
{
    current_texcoords.first = i;
    current_texcoords.second = j;
}

void ImmediateModeProvider::AddVertex(float x, float y, float z)
{
    SORE_Graphics::vertex temp;
    temp.r = current_color.GetComponent(SORE_Graphics::RED);
    temp.g = current_color.GetComponent(SORE_Graphics::GREEN);
    temp.b = current_color.GetComponent(SORE_Graphics::BLUE);
    temp.a = current_color.GetComponent(SORE_Graphics::ALPHA);

    temp.tex0i = current_texcoords.first;
    temp.tex0j = current_texcoords.second;

    temp.x = x;
    temp.y = y;
    temp.z = z;

    vertices.push_back(temp);
}

void ImmediateModeProvider::CreateRenderableFromData()
{
    if(!indices.size())
        return;

    SORE_Graphics::GeometryChunkPtr geometry(new SORE_Graphics::GeometryChunk(vertices.size(), indices.size(), current_primitive_type));
    std::copy(vertices.begin(), vertices.end(), geometry->GetVertices());
    std::copy(indices.begin(),  indices.end(),  geometry->GetIndices());

    buffer_manager.GeometryAdded(geometry, SORE_Graphics::STREAM);

    SORE_Graphics::Renderable r(geometry, current_shader, SORE_Graphics::TransformationPtr(new SORE_Math::Matrix4<float> ()), current_blend_mode);
    r.AddTexture("texture", current_texture);
    r.AddKeyword("game");

    renderables.push_back(r);

    vertices.clear();
    indices.clear();
}