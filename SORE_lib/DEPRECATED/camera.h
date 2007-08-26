#ifndef  __CAMERA_H__
#define  __CAMERA_H__

#include "allgl.h"

const int AXIS_X = 1;
const int AXIS_Y = 2;
const int AXIS_Z = 3;

struct RotationLimit
{
	RotationLimit(): upperLimit(false), lowerLimit(false) {}
	GLfloat upper;
	bool upperLimit;
	GLfloat lower;
	bool lowerLimit;
};

class Camera
{
	public:
		Camera();
		void Translate(GLfloat i, GLfloat j, GLfloat k);
		void Rotate(GLfloat i, GLfloat j, GLfloat k);
		void TransformView();
		void SetRotationUpperLimit(int axis, GLfloat value);
		void SetRotationLowerLimit(int axis, GLfloat value);
		GLfloat* GetRotation();
		GLfloat* GetPosition();
		void Print();
	private:
	protected:
		GLfloat x, y, z;
		GLfloat xr, yr, zr;
		RotationLimit xl,yl,zl;
};

#endif //__CAMERA_H__
