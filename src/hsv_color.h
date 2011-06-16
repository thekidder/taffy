#ifndef HSV_COLOR_H
#define HSV_COLOR_H

#include <sore_color.h>

class HSVColor
{
public:
    HSVColor(float h, float s, float v, float a = 1.0f);

    HSVColor Complement() const;
    HSVColor Tint(float amount) const;
    HSVColor Shade(float amount) const;

    const SORE_Graphics::Color& RGBColor() const;
private:
    void CacheRGB();

    float hue, saturation, value;
    float alpha;
    SORE_Graphics::Color rgb;
};

#endif