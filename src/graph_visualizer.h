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
        std::pair<double, double> input_range_, int num_series, int history_size_);

    void AddDatum(int series, double datum);

    // set a comment to be rendered
    void SetComment(const std::string& comment_);
private:
    virtual void UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode);
    virtual bool ProcessEvents(SORE_Kernel::Event*) { return false; }

    std::pair<double, double> input_range;

    typedef std::list<double> data_container;
    std::vector<data_container> data;
    int history_size;

    SORE_Resource::GLSLShaderPtr shader;
    SORE_Resource::GLSLShaderPtr font_shader;
    SORE_Resource::FontPtr face;

    std::string comment;
};

#endif