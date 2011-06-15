#ifndef PIVOT_CAMERA_H
#define PIVOT_CAMERA_H

#include <sore_matrix4x4.h>
#include <sore_quaternion.h>

class PivotCamera
{
public:
    PivotCamera();

    void Rotate(float x, float y);
    SORE_Math::Matrix4<float> Matrix() const;
private:
    float x_rads, y_rads;
};

#endif