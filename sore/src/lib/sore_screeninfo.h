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

#ifndef  SORE_SCREENINFO_H
#define  SORE_SCREENINFO_H

#include <sore_math.h>

namespace SORE_Graphics
{
    enum ProjectionType {NONE, ORTHO, ORTHO2D, PERSPECTIVE};

    struct SORE_EXPORT ProjectionInfo
    {
        ProjectionInfo(float left, float right) { type = ORTHO2D;
            this->left = left; this->right = right;
            useScreenCoords = false;
            useScreenRatio = true; }
        ProjectionInfo() {type = NONE;
            fov = ratio = znear = zfar = top = bottom = left = right = 0.0;
            useScreenCoords = useScreenRatio = false; }
        ProjectionType type;
        float fov,ratio;
        float znear, zfar;
        float top, bottom, left, right;
        //if this is true, and type of projection is ortho2d, use width/height
        //for projection
        bool useScreenCoords;
        //if true, uses screen ratio (for ortho, gets top/bottom by dividing
        //left/right by ratio)
        bool useScreenRatio;
    };

    bool operator==(const ProjectionInfo& one, const ProjectionInfo& two);
    bool operator!=(const ProjectionInfo& one, const ProjectionInfo& two);

    struct SORE_EXPORT ScreenInfo
    {
        int width, height;
        float ratio; //set by SORE_Screen after screen is created
        bool showCursor;
        bool fullscreen;
        bool resizable;
        bool useNativeResolution; //supercedes width, height, ratio
    };

    ProjectionInfo SORE_EXPORT SetupProjection(const ProjectionInfo& pi, const ScreenInfo& si);

    SORE_Math::Vector2<float> SORE_EXPORT ScreenToProjection(
        ScreenInfo screen, ProjectionInfo proj,
        SORE_Math::Vector2<int> pos);
    SORE_Math::Vector2<int> SORE_EXPORT ProjectionToScreen(
        ScreenInfo screen, ProjectionInfo proj,
        SORE_Math::Vector2<float> pos);
}

#endif
