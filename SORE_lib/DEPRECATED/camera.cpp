#include "camera.h"

SORE_Graphics::Camera::Camera()
{
	x = xr = y = yr = z = zr = 0.0f;
	z = 5.0f;
}

void SORE_Graphics::Camera::Rotate(GLfloat i, GLfloat j, GLfloat k)
{
	xr += i;
	yr += j;
	zr += k;
	
	if(xr>xl.upper && xl.upperLimit)
		xr = xl.upper;
	else if(xr<xl.lower && xl.lowerLimit)
		xr = xl.lower;
	if(yr>yl.upper && yl.upperLimit)
		yr = yl.upper;
	else if(yr<yl.lower && yl.lowerLimit)
		yr = yl.lower;
	if(zr>zl.upper && zl.upperLimit)
		zr = zl.upper;
	else if(zr<zl.lower && zl.lowerLimit)
		zr = zl.lower;
	
	if(xr >  180.0f) xr = -180.0f;
	if(yr >  180.0f) yr = -180.0f;
	if(zr >  180.0f) zr = -180.0f;
	if(xr < -180.0f) xr =  180.0f;
	if(yr < -180.0f) yr =  180.0f;
	if(zr < -180.0f) zr =  180.0f;
}

void SORE_Graphics::Camera::SetRotationUpperLimit(int axis, GLfloat limit)
{
	if(axis<1 || axis>3) return;
	
	switch(axis)
	{
		case AXIS_X:
			xl.upperLimit = true;
			xl.upper = limit;
			break;
		case AXIS_Y:
			yl.upperLimit = true;
			yl.upper = limit;
			break;
		case AXIS_Z:
			zl.upperLimit = true;
			zl.upper = limit;
			break;
	}
}

void SORE_Graphics::Camera::SetRotationLowerLimit(int axis, GLfloat limit)
{
	if(axis<1 || axis>3) return;
	
	switch(axis)
	{
		case AXIS_X:
			xl.lowerLimit = true;
			xl.lower = limit;
			break;
		case AXIS_Y:
			yl.lowerLimit = true;
			yl.lower = limit;
			break;
		case AXIS_Z:
			yl.lowerLimit = true;
			yl.lower = limit;
			break;
	}
}

void SORE_Graphics::Camera::Translate(GLfloat i, GLfloat j, GLfloat k)
{
	//std::cout << "k: " << k << std::endl;
	x += i;
	y += j;
	z += k;
}

GLfloat* SORE_Graphics::Camera::GetRotation()
{
	static GLfloat vector[3]
	vector[0] = xr;
	vector[1] = yr;
	vector[2] = zr;
	return vector;
}

GLfloat* SORE_Graphics::Camera::GetPosition()
{
	static GLfloat vector[3]
	vector[0] = x;
	vector[1] = y;
	vector[2] = z;
	return vector;
}

void SORE_Graphics::Camera::TransformView()
{
	::glLoadIdentity();
	::glRotatef(-xr, 1.0f, 0.0f, 0.0f);
	::glRotatef(-yr, 0.0f, 1.0f, 0.0f);
	::glRotatef(-zr, 0.0f, 0.0f, 1.0f);
	::glTranslatef(-x, -y, -z);
}
