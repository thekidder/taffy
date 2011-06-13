#ifndef SPECTRUM_VISUALIZER_H
#define SPECTRUM_VISUALIZER_H

#include "spectrum.h"

#include <sore_widget.h>

class SpectrumVisualizer : public SORE_GUI::Widget
{
public:
    SpectrumVisualizer(
        SORE_GUI::SVec size, SORE_GUI::SVec position, SORE_GUI::Widget* parent,
        Spectrum* spectrum_);

    void SetSpectrum(Spectrum* spectrum_);
private:
    virtual void UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode);
    virtual bool ProcessEvents(SORE_Kernel::Event*) { return false; }

    Spectrum* spectrum;

    SORE_Resource::GLSLShaderPtr shader;
    SORE_Resource::GLSLShaderPtr font_shader;
    SORE_Resource::FontPtr face;
};

#endif