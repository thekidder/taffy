/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#include <sore_geometry.h>
#include <sore_logger.h>
#include <sore_sliderwidget.h>

namespace SORE_GUI
{
    SliderWidget::SliderWidget(SVec s, SVec p, int min, int max,
                               Widget* par)
        : FrameWidget(s, p, SCALE_ALL, par), sliderChunk(0), dragged(0),
          minimum(min), maximum(max), current((max -  min)/2)
    {
        std::string styleDir("data/");
        styleDir += StyleName() + "/";

        UpdatePosition();
        SetBorderSizes(16.0f, 16.0f, 16.0f, 16.0f);
        SetTexture(bg);
    }

    SliderWidget::~SliderWidget()
    {
        delete sliderChunk;
    }

    void SliderWidget::ConnectChange(boost::function<void (int)> c)
    {
        onChange.connect(c);
    }

    int SliderWidget::GetValue() const
    {
        return current;
    }

    void SliderWidget::SetValue(int value)
    {
        value = value < minimum ? minimum : value;
        value = value > maximum ? maximum : value;
        current = value;
        UpdateSlider();
        onChange(value);
    }

    /*SORE_Graphics::render_list SliderWidget::GetThisRenderList()
    {
        SORE_Graphics::render_list list;

        std::vector<SORE_Graphics::GeometryChunk*> frame = GetChunks();
        std::vector<SORE_Graphics::GeometryChunk*>::iterator it;
        for(it = frame.begin();it!=frame.end();++it)
        {
            list.push_back(std::make_pair(&GetPositionMatrix(), *it));
        }

        list.push_back(std::make_pair(&sliderMat, sliderChunk));

        return list;
    }
    */

    bool SliderWidget::ProcessEvents(const SORE_Kernel::Event& e)
    {
        float pos = ValueToX(current);
        switch(e.type)
        {
        case SORE_Kernel::MOUSEBUTTONDOWN:
            if(e.mouse.x < pos || e.mouse.x > pos+GetSize(VERTICAL)/8.0f)
            {
                SetValue(XToValue(static_cast<float>(e.mouse.x)));
            }
            dragged = true;
            return true;
            break;
        case SORE_Kernel::MOUSEBUTTONUP:
            dragged = false;
            return true;
            break;
        case SORE_Kernel::MOUSEMOVE:
            if(dragged)
            {
                SetValue(XToValue(static_cast<float>(e.mouse.x)));
                return true;
            }
            break;
        default:
            break;
        }
        return false;
    }

    void SliderWidget::UpdatePosition()
    {
        delete sliderChunk;
        float sliderHeight = static_cast<float>(GetSize(VERTICAL));
        float sliderWidth = sliderHeight / 8.0f;

        SORE_Math::Rect<float> sliderBounds
            (-sliderWidth, sliderWidth, 0, sliderHeight);
        //TODO:fixme!
        //sliderChunk = new SORE_Graphics::GeometryChunk(slider, shader, sliderBounds);

        UpdateSlider();
    }

    void SliderWidget::UpdateSlider()
    {
        float pos = ValueToX(current);
        sliderMat = GetPositionMatrix() *
            SORE_Math::Matrix4<float>::GetTranslation(pos, 0.0f,
                                                      GetLayer() +
                                                      LAYER_SEPARATION/2.0f);
    }

    float SliderWidget::ValueToX(int value) const
    {
        value = value < minimum ? minimum : value;
        value = value > maximum ? maximum : value;
        return static_cast<float>(value - minimum) /
            static_cast<float>(maximum - minimum) *
            (GetSize(HORIZONTAL) - 32.0f) + 16.0f;
    }

    int SliderWidget::XToValue(float x) const
    {
        int range = maximum - minimum;
        int value =  static_cast<int>( (x - 16.0f) /
                                       (GetSize(HORIZONTAL)-32.0f) * range - minimum);
        value = value < minimum ? minimum : value;
        value = value > maximum ? maximum : value;
        return value;
    }
}
