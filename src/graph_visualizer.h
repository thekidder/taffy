#ifndef GRAPH_VISUALIZER_H
#define GRAPH_VISUALIZER_H

#include "immediate_mode_provider.h"

#include <list>
#include <vector>

class GraphVisualizer
{
public:
    GraphVisualizer(
        std::pair<float, float> position_, std::pair<float, float> size_, 
        std::pair<float, float> input_range_, int num_series, int history_size_);

    void AddDatum(int series, float datum);
    void Render(ImmediateModeProvider& imm_mode);
private:
    std::pair<float, float> position;
    std::pair<float, float> size;
    std::pair<float, float> input_range;

    typedef std::list<float> data_container;
    std::vector<data_container> data;
    int history_size;
};

#endif