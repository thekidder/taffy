#ifndef  __SORE_CAMERA_H__
#define  __SORE_CAMERA_H__

#include "sore_allgl.h"
#include "sore_kernel.h"
#include "sore_input.h"

const int AXIS_X = 1;
const int AXIS_Y = 2;
const int AXIS_Z = 3;

namespace SORE_Graphics
{
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
			void LogPosition();
		private:
		protected:
			GLfloat x, y, z;
			GLfloat xr, yr, zr;
			RotationLimit xl,yl,zl;
	};
	
	class CameraTask : public SORE_Kernel::Task
	{
		public:
			CameraTask();
			CameraTask(Camera* camera);
			~CameraTask() {}
			
			void Pause();
			void Resume();
			void Frame(int elapsedTime);
			
			const char* GetName() const {return "Camera Task";}
			
			void SetCamera(Camera* camera);
			bool CameraCallback(SORE_Kernel::Event* event);
		protected:
			Camera* cam;
			bool forward,back,left,right;
			bool grabbed;
	};
}

#endif //__SORE_CAMERA_H__
