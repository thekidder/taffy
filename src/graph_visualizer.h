#ifndef GRAPH_VISUALIZER_H
#define GRAPH_VISUALIZER_H

#include "immediate_mode_provider.h"

#include <list>

class GraphVisualizer
{
public:
    GraphVisualizer(
        std::pair<float, float> position_, std::pair<float, float> size_, 
        std::pair<float, float> input_range_, int history_size_);

    void AddDatum(float datum);
    void Render(ImmediateModeProvider& imm_mode);
private:
    std::pair<float, float> position;
    std::pair<float, float> size;
    std::pair<float, float> input_range;

    typedef std::list<float> data_container;
    data_container data;
    int history_size;
};

#endif