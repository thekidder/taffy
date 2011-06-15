#include "pivot_camera.h"

PivotCamera::PivotCamera() : x_rads(0.0f), y_rads(0.0f)
{
}

void PivotCamera::Rotate(float x, float y)
{
    x_rads += x;
    y_rads += y;
}

SORE_Math::Matrix4<float> PivotCamera::Matrix() const
{
    SORE_Math::Quaternion<float> rotation = SORE_Math::Quaternion<float>::GetRotation(x_rads, SORE_Math::Vector3<float>(0.0f, 1.0f, 0.0f));
    rotation *= SORE_Math::Quaternion<float>::GetRotation(y_rads, SORE_Math::Vector3<float>(1.0f, 0.0f, 0.0f));

    SORE_Math::Matrix4<float> m = SORE_Math::Matrix4<float>::GetTranslation(0.0f, 0.0f, -10.0f);
    m *= rotation.GetMatrix();

    return m;
}