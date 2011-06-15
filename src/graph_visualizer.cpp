#include "graph_visualizer.h"
#include "utility.h"

#include <sore_font_loader.h>
#include <sore_glslshader_loader.h>
#include <sore_texture2d_loader.h>

GraphVisualizer::GraphVisualizer(
        SORE_GUI::SVec size, SORE_GUI::SVec position, SORE_GUI::Widget* parent,
        std::pair<double, double> input_range_, int num_series, size_t history_size_)
        : Widget(size, position, parent), input_range(input_range_),
        data(num_series), history_size(history_size_)
{
    shader = shaderCache.Get("untextured.shad");
    font_shader = shaderCache.Get("default.shad");
    face = fontCache.Get("ix/LiberationSans-Regular.ttf");
}

void GraphVisualizer::AddDatum(int series, double datum)
{
    if(data[series].size() == history_size)
        data[series].pop_front();

    data[series].push_back(MapToRange(datum, input_range, k_std_range_d));
}

void GraphVisualizer::SetComment(const std::string& comment_)
{
    comment = comment_;
}

void GraphVisualizer::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
{
    Double_range_t render_range(0.0, static_cast<double>(GetSize(SORE_GUI::VERTICAL)));
    float width  = static_cast<float>(GetSize(SORE_GUI::HORIZONTAL));
    float height = static_cast<float>(GetSize(SORE_GUI::VERTICAL));

    // draw background
    imm_mode.SetTransform(SORE_Graphics::TransformationPtr(new SORE_Math::Matrix4<float>(GetPositionMatrix())));
    imm_mode.SetShader(shader);
    imm_mode.SetTexture(SORE_Resource::Texture2DPtr());
    imm_mode.SetColor(SORE_Graphics::Grey);
    imm_mode.SetBlendMode(SORE_Graphics::BLEND_SUBTRACTIVE);
    imm_mode.DrawQuad(
        0.0f,  0.0f,   0.0f,
        0.0f,  height, 0.0f,
        width, 0.0f,   0.0f,
        width, height, 0.0f);

    for(size_t i = 0; i < data.size(); ++i)
    {
        // can't render a line with less than 2 endpoints
        if(data[i].size() < 2)
            return;

        switch(i)
        {
        case 0:
            imm_mode.SetColor(SORE_Graphics::Red);
            break;
        case 1:
            imm_mode.SetColor(SORE_Graphics::Green);
            break;
        case 2:
            imm_mode.SetColor(SORE_Graphics::Blue);
            break;
        case 3:
            imm_mode.SetColor(SORE_Graphics::Yellow);
            break;
        }

        double x_start = GetSize(SORE_GUI::HORIZONTAL);
        double x_decrement = static_cast<double>(GetSize(SORE_GUI::HORIZONTAL)) / history_size;

        double x_last = x_start;
        double y_last = GetSize(SORE_GUI::VERTICAL) - MapToRange(data[i].back(), k_std_range_d, render_range);

        double x = x_last;
        for(data_container::reverse_iterator it = ++data[i].rbegin(); it != data[i].rend(); ++it)
        {
            x -= x_decrement;
            double y = GetSize(SORE_GUI::VERTICAL) - MapToRange(*it, k_std_range_d, render_range);

            imm_mode.DrawLine(
                static_cast<float>(x_last), static_cast<float>(y_last), SORE_GUI::LAYER_SEPARATION / 3.0f,
                static_cast<float>(x),      static_cast<float>(y),      SORE_GUI::LAYER_SEPARATION / 3.0f);

            x_last = x;
            y_last = y;
        }
    }

    imm_mode.SetColor(SORE_Graphics::White);
    imm_mode.SetShader(font_shader);
    imm_mode.DrawString(
        static_cast<float>(GetSize(SORE_GUI::HORIZONTAL) - face->Width(24, comment)),
        static_cast<float>(GetSize(SORE_GUI::VERTICAL) - 24),
        2.0f * SORE_GUI::LAYER_SEPARATION / 3.0f,
        face, 24u, comment);
}
