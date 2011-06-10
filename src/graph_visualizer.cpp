#include "graph_visualizer.h"
#include "utility.h"

GraphVisualizer::GraphVisualizer(
        SORE_GUI::SVec size, SORE_GUI::SVec position, SORE_GUI::Widget* parent,
        SORE_Resource::ResourcePool& pool,
        std::pair<float, float> input_range_, int num_series, int history_size_)
        : Widget(size, position, parent), input_range(input_range_), 
        data(num_series), history_size(history_size_)
{
    shader = pool.GetResource<SORE_Graphics::GLSLShader>("data/Shaders/untextured.shad");
}

void GraphVisualizer::AddDatum(int series, float datum)
{
    if(data[series].size() == history_size)
        data[series].pop_front();

    data[series].push_back(MapToRange(datum, input_range, k_std_range));
}

void GraphVisualizer::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
{
    Float_range_t render_range(0.0f, GetSize(SORE_GUI::VERTICAL));

    // draw background
    imm_mode.SetTransform(SORE_Graphics::TransformationPtr(new SORE_Math::Matrix4<float>(GetPositionMatrix())));
    imm_mode.SetShader(shader);
    imm_mode.SetColor(SORE_Graphics::Grey);
    imm_mode.SetBlendMode(SORE_Graphics::BLEND_SUBTRACTIVE);
    imm_mode.DrawQuad(
        0.0f,                          0.0f,                        0.0f,
        0.0f,                          GetSize(SORE_GUI::VERTICAL), 0.0f,
        GetSize(SORE_GUI::HORIZONTAL), 0.0f,                        0.0f,
        GetSize(SORE_GUI::HORIZONTAL), GetSize(SORE_GUI::VERTICAL), 0.0f);

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

        float x_start = GetSize(SORE_GUI::HORIZONTAL);
        float x_decrement = static_cast<float>(GetSize(SORE_GUI::HORIZONTAL)) / history_size;

        float x_last = x_start;
        float y_last = MapToRange(data[i].back(), k_std_range, render_range);

        float x = x_last;
        for(data_container::reverse_iterator it = ++data[i].rbegin(); it != data[i].rend(); ++it)
        {
            x -= x_decrement;
            float y = MapToRange(*it, k_std_range, render_range);

            imm_mode.DrawLine(x_last, y_last, 0.2f, x, y, 0.2f);

            x_last = x;
            y_last = y;
        }
    }
}