#include "hsv_color.h"

#include <algorithm>
#include <cmath>

HSVColor::HSVColor(float h, float s, float v, float a)
    : hue(h), saturation(s), value(v), alpha(a)
{
    CacheRGB();
}

HSVColor HSVColor::Complement() const
{
    float h = hue - 0.5f;
    if(h < 0.0f) 
        h += 1.0f;
    return HSVColor(h, saturation, value, alpha);
}

HSVColor HSVColor::Tint(float amount) const
{
    float s = saturation - amount;
    s = std::max(0.0f, s);

    return HSVColor(hue, s, value, alpha);
}

HSVColor HSVColor::Shade(float amount) const
{
    float v = value - amount;
    v = std::max(0.0f, v);

    return HSVColor(hue, saturation, v, alpha);
}

const SORE_Graphics::Color& HSVColor::RGBColor() const
{
    return rgb;
}

void HSVColor::CacheRGB()
{
    float chroma = value * saturation; 
    int h_prime  = static_cast<int>((hue * 360.0f) / 60.0f);
    float x      = chroma * (1.0f - fabs(h_prime % 2 - 1.0f));

    if(0 <= h_prime && h_prime < 1)
        rgb = SORE_Graphics::Color(chroma, x, 0.0f);
    else if(1 <= h_prime && h_prime < 2)
        rgb = SORE_Graphics::Color(x, chroma, 0.0f);
    else if(2 <= h_prime && h_prime < 3)
        rgb = SORE_Graphics::Color(0.0f, chroma, x);
    else if(3 <= h_prime && h_prime < 4)
        rgb = SORE_Graphics::Color(0.0f, x, chroma);
    else if(4 <= h_prime && h_prime < 5)
        rgb = SORE_Graphics::Color(x, 0.0f, chroma);
    else if(5 <= h_prime && h_prime < 6)
        rgb = SORE_Graphics::Color(chroma, 0.0f, x);
    else
        rgb = SORE_Graphics::Color(1.0f);

    rgb += (value - chroma);
    rgb.SetComponent(SORE_Graphics::ALPHA, alpha);
}