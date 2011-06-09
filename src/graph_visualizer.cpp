#include "graph_visualizer.h"
#include "utility.h"

GraphVisualizer::GraphVisualizer(
        std::pair<float, float> position_, std::pair<float, float> size_, 
        std::pair<float, float> input_range_, int history_size_)
        : position(position_), size(size_), input_range(input_range_), history_size(history_size_)
{
}

void GraphVisualizer::AddDatum(float datum)
{
    if(data.size() == history_size)
        data.pop_front();

    data.push_back(MapToRange(datum, input_range, k_std_range));
}

void GraphVisualizer::Render(ImmediateModeProvider& imm_mode)
{
    Float_range_t render_range(position.second, position.second + size.second);

    // draw background
    imm_mode.SetColor(SORE_Graphics::Grey);
    imm_mode.DrawQuad(
        position.first,              position.second,               0.0f,
        position.first,              position.second + size.second, 0.0f,
        position.first + size.first, position.second,               0.0f,
        position.first + size.first, position.second + size.second, 0.0f);

    // can't render a line with less than 2 endpoints
    if(data.size() < 2)
        return;

    imm_mode.SetColor(SORE_Graphics::Red);

    float x_start = position.first + size.first;
    float x_decrement = size.first / history_size;

    float x_last = x_start;
    float y_last = MapToRange(data.back(), k_std_range, render_range);

    float x = x_last;
    for(data_container::reverse_iterator it = ++data.rbegin(); it != data.rend(); ++it)
    {
        x -= x_decrement;
        float y = MapToRange(*it, k_std_range, render_range);

        imm_mode.DrawLine(x_last, y_last, 0.2f, x, y, 0.2f);

        x_last = x;
        y_last = y;
    }
}