#include "spectrum_visualizer.h"
#include "utility.h"

SpectrumVisualizer::SpectrumVisualizer(
        SORE_GUI::SVec size, SORE_GUI::SVec position, SORE_GUI::Widget* parent,
        SORE_Resource::ResourcePool& pool, Spectrum* spectrum_)
        : Widget(size, position, parent), spectrum(spectrum_)
{
    shader = pool.GetResource<SORE_Graphics::GLSLShader>("data/Shaders/untextured.shad");
}

void SpectrumVisualizer::SetSpectrum(Spectrum* spectrum_)
{
    spectrum = spectrum_;
}

const Float_range_t k_db_range(-60.0f, 0.0f);

void SpectrumVisualizer::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
{
    imm_mode.SetTransform(SORE_Graphics::TransformationPtr(new SORE_Math::Matrix4<float>(GetPositionMatrix())));
    imm_mode.SetShader(shader);
    imm_mode.SetColor(SORE_Graphics::Grey);
    imm_mode.SetBlendMode(SORE_Graphics::BLEND_SUBTRACTIVE);
    imm_mode.DrawQuad(
        0.0f,                          0.0f,                        0.0f,
        0.0f,                          GetSize(SORE_GUI::VERTICAL), 0.0f,
        GetSize(SORE_GUI::HORIZONTAL), 0.0f,                        0.0f,
        GetSize(SORE_GUI::HORIZONTAL), GetSize(SORE_GUI::VERTICAL), 0.0f);

    Float_range_t display_range(0.0f, GetSize(SORE_GUI::VERTICAL));
    float width = (GetSize(SORE_GUI::HORIZONTAL) / spectrum->NumBuckets()) / 2.0f;
    float height = GetSize(SORE_GUI::VERTICAL);
    for(int i = 0; i < spectrum->NumBuckets(); ++i)
    {
        float left  = spectrum->Value(i, CHANNEL_LEFT);
        float right = spectrum->Value(i, CHANNEL_RIGHT);
        left  = MapToRange(left,  k_db_range, display_range);
        right = MapToRange(right, k_db_range, display_range);

        imm_mode.SetColor(SORE_Graphics::Green);
        imm_mode.DrawQuad(
            width * i * 2.0f,       height,        0.0f,
            width * i * 2.0f,       height - left, 0.0f,
            width * (i * 2.0f + 1), height,        0.0f,
            width * (i * 2.0f + 1), height - left, 0.0f);

        imm_mode.SetColor(SORE_Graphics::Red);
        imm_mode.DrawQuad(
            width * (i * 2.0f + 1), height,         0.0f,
            width * (i * 2.0f + 1), height - right, 0.0f,
            width * (i * 2.0f + 2), height,         0.0f,
            width * (i * 2.0f + 2), height - right, 0.0f);
    }
}