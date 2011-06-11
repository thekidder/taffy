#ifndef GRAPH_VISUALIZER_H
#define GRAPH_VISUALIZER_H

#include <sore_widget.h>

#include <list>
#include <vector>

class GraphVisualizer : public SORE_GUI::Widget
{
public:
    GraphVisualizer(
        SORE_GUI::SVec size, SORE_GUI::SVec position, SORE_GUI::Widget* parent,
        SORE_Resource::ResourcePool& pool,
        std::pair<float, float> input_range_, int num_series, int history_size_);

    void AddDatum(int series, float datum);

    // set a comment to be rendered
    void SetComment(const std::string& comment_);
private:
    virtual void UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode);
    virtual bool ProcessEvents(SORE_Kernel::Event*) { return false; }

    std::pair<float, float> input_range;

    typedef std::list<float> data_container;
    std::vector<data_container> data;
    int history_size;

    SORE_Graphics::GLSLShaderPtr shader;
    SORE_Graphics::GLSLShaderPtr font_shader;
    SORE_Font::FontPtr face;

    std::string comment;
};

#endif