#include "spectrum_visualizer.h"
#include "utility.h"

#include <sore_font_loader.h>
#include <sore_material_loader.h>
#include <sore_texture2d_loader.h>

#include <boost/format.hpp>

SpectrumVisualizer::SpectrumVisualizer(
        SORE_GUI::SVec size, SORE_GUI::SVec position, SORE_GUI::Widget* parent,
        Spectrum* spectrum_)
        : Widget(size, position, parent), spectrum(spectrum_)
{
    material = materialCache.Get("untextured.json");
    font_material = materialCache.Clone("default.shad");
    face = fontCache.Get("ix/LiberationSans-Regular.ttf");
}

void SpectrumVisualizer::SetSpectrum(Spectrum* spectrum_)
{
    spectrum = spectrum_;
}

const Double_range_t k_db_range(-60.0, 0.0);

void SpectrumVisualizer::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
{
    float width  = static_cast<float>(GetSize(SORE_GUI::HORIZONTAL));
    float height = static_cast<float>(GetSize(SORE_GUI::VERTICAL));

    imm_mode.SetTransform(SORE_Graphics::MatrixPtr(new SORE_Math::Matrix4<float>(GetPositionMatrix())));
    imm_mode.SetMaterial(material);
    imm_mode.SetColor(SORE_Graphics::Grey);
    imm_mode.DrawQuad(
        0.0f,  0.0f,   0.0f,
        0.0f,  height, 0.0f,
        width, 0.0f,   0.0f,
        width, height, 0.0f);

    Double_range_t display_range(0.0, static_cast<double>(GetSize(SORE_GUI::VERTICAL)));
    float bucket_width = (static_cast<float>(GetSize(SORE_GUI::HORIZONTAL)) / spectrum->NumBuckets()) / 2.0f;
    for(size_t i = 0; i < spectrum->NumBuckets(); ++i)
    {
        double left  = spectrum->Value(i, CHANNEL_LEFT);
        double right = spectrum->Value(i, CHANNEL_RIGHT);
        left  = MapToRange(left,  k_db_range, display_range);
        right = MapToRange(right, k_db_range, display_range);

        float h_left = static_cast<float>(left);
        float h_right = static_cast<float>(right);

        imm_mode.SetColor(SORE_Graphics::Green);
        imm_mode.DrawQuad(
            bucket_width * i * 2.0f,          height,          SORE_GUI::LAYER_SEPARATION / 3.0f,
            bucket_width * i * 2.0f,          height - h_left, SORE_GUI::LAYER_SEPARATION / 3.0f,
            bucket_width * (i * 2.0f + 1.0f), height,          SORE_GUI::LAYER_SEPARATION / 3.0f,
            bucket_width * (i * 2.0f + 1.0f), height - h_left, SORE_GUI::LAYER_SEPARATION / 3.0f);

        imm_mode.SetColor(SORE_Graphics::Red);
        imm_mode.DrawQuad(
            bucket_width * (i * 2.0f + 1.0f), height,           SORE_GUI::LAYER_SEPARATION / 3.0f,
            bucket_width * (i * 2.0f + 1.0f), height - h_right, SORE_GUI::LAYER_SEPARATION / 3.0f,
            bucket_width * (i * 2.0f + 2.0f), height,           SORE_GUI::LAYER_SEPARATION / 3.0f,
            bucket_width * (i * 2.0f + 2.0f), height - h_right, SORE_GUI::LAYER_SEPARATION / 3.0f);
    }

    imm_mode.SetColor(SORE_Graphics::White);
    imm_mode.SetMaterial(font_material);
    for(size_t i = 0; i < spectrum->NumBuckets(); ++i)
    {
        std::string str = (boost::format("%.0f-%.0fHz") % spectrum->HzRange(i).first % spectrum->HzRange(i).second).str();
        float str_width = face->Width(16, str);
        if(bucket_width * 2.0 >= str_width)
        {
            float text_y = static_cast<float>(GetSize(SORE_GUI::VERTICAL) - 24);
            imm_mode.DrawString(
                (bucket_width * 2.0f - str_width) / 2.0f + bucket_width * i * 2.0f, 
                text_y, 
                2.0f * SORE_GUI::LAYER_SEPARATION / 3.0f, 
                face, 16u, str);
        }
    }
}