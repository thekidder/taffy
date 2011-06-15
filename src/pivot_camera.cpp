#include "pivot_camera.h"

PivotCamera::PivotCamera()
{
}

void PivotCamera::Rotate(float x, float y)
{
    q_x.Rotate(x, SORE_Math::Vector3<float>(0.0f, 1.0f, 0.0f));
    q_y.Rotate(y, SORE_Math::Vector3<float>(1.0f, 0.0f, 0.0f));
}

SORE_Math::Matrix4<float> PivotCamera::Matrix() const
{
    SORE_Math::Matrix4<float> m = SORE_Math::Matrix4<float>::GetTranslation(0.0f, 0.0f, -5.0f);
    m *= (q_x * q_y).GetMatrix();

    return m;
}