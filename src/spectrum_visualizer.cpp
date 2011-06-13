#include "spectrum_visualizer.h"
#include "utility.h"

#include <boost/format.hpp>

SpectrumVisualizer::SpectrumVisualizer(
        SORE_GUI::SVec size, SORE_GUI::SVec position, SORE_GUI::Widget* parent,
        Spectrum* spectrum_)
        : Widget(size, position, parent), spectrum(spectrum_)
{
    /*shader = pool.GetResource<SORE_Graphics::GLSLShader>("data/Shaders/untextured.shad");
    font_shader = pool.GetResource<SORE_Graphics::GLSLShader>("data/Shaders/default.shad");
    face = pool.GetResource<SORE_Font::Font>("data/ix_style/LiberationSans-Regular.ttf");*/
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
    imm_mode.SetTexture(SORE_Resource::Texture2DPtr());
    imm_mode.SetColor(SORE_Graphics::Grey);
    imm_mode.SetBlendMode(SORE_Graphics::BLEND_SUBTRACTIVE);
    imm_mode.DrawQuad(
        0.0f,                          0.0f,                        0.0f,
        0.0f,                          GetSize(SORE_GUI::VERTICAL), 0.0f,
        GetSize(SORE_GUI::HORIZONTAL), 0.0f,                        0.0f,
        GetSize(SORE_GUI::HORIZONTAL), GetSize(SORE_GUI::VERTICAL), 0.0f);

    Float_range_t display_range(0.0f, GetSize(SORE_GUI::VERTICAL));
    double width = (static_cast<double>(GetSize(SORE_GUI::HORIZONTAL)) / spectrum->NumBuckets()) / 2.0;
    float height = GetSize(SORE_GUI::VERTICAL);
    for(int i = 0; i < spectrum->NumBuckets(); ++i)
    {
        float left  = spectrum->Value(i, CHANNEL_LEFT);
        float right = spectrum->Value(i, CHANNEL_RIGHT);
        left  = MapToRange(left,  k_db_range, display_range);
        right = MapToRange(right, k_db_range, display_range);

        imm_mode.SetColor(SORE_Graphics::Green);
        imm_mode.DrawQuad(
            width * i * 2.0,       height,        SORE_GUI::LAYER_SEPARATION / 3.0,
            width * i * 2.0,       height - left, SORE_GUI::LAYER_SEPARATION / 3.0,
            width * (i * 2.0 + 1), height,        SORE_GUI::LAYER_SEPARATION / 3.0,
            width * (i * 2.0 + 1), height - left, SORE_GUI::LAYER_SEPARATION / 3.0);

        imm_mode.SetColor(SORE_Graphics::Red);
        imm_mode.DrawQuad(
            width * (i * 2.0 + 1), height,         SORE_GUI::LAYER_SEPARATION / 3.0,
            width * (i * 2.0 + 1), height - right, SORE_GUI::LAYER_SEPARATION / 3.0,
            width * (i * 2.0 + 2), height,         SORE_GUI::LAYER_SEPARATION / 3.0,
            width * (i * 2.0 + 2), height - right, SORE_GUI::LAYER_SEPARATION / 3.0);
    }

    imm_mode.SetColor(SORE_Graphics::White);
    imm_mode.SetShader(font_shader);
    for(int i = 0; i < spectrum->NumBuckets(); ++i)
    {
        std::string str = (boost::format("%.0f-%.0fHz") % spectrum->HzRange(i).first % spectrum->HzRange(i).second).str();
        float str_width = face->Width(16, str);
        if(width * 2.0 >= str_width)
        {
            imm_mode.DrawString(
                (width * 2.0 - str_width) / 2.0 + width * i * 2.0, 
                GetSize(SORE_GUI::VERTICAL) - 24, 
                2.0 * SORE_GUI::LAYER_SEPARATION / 3.0, 
                face, 16, str);
        }
    }
}