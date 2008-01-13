#include "sore_defines.h"
#include "sore_camera.h"
#include "sore_logger.h"
#include <cmath>

SORE_Graphics::Camera::Camera()
{
	x = xr = y = yr = z = zr = 0.0f;
	z = 0.0f;
	xl.upperLimit = xl.lowerLimit = false;
	yl.upperLimit = yl.lowerLimit = false;
	zl.upperLimit = zl.lowerLimit = false;
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
	static GLfloat vector[3];
	vector[0] = xr;
	vector[1] = yr;
	vector[2] = zr;
	return vector;
}

GLfloat* SORE_Graphics::Camera::GetPosition()
{
	static GLfloat vector[3];
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

void SORE_Graphics::Camera::LogPosition()
{
	ENGINE_LOG(SORE_Logging::LVL_DEBUG1, boost::format("position: (%f, %f, %f) rotation: (%f, %f, %f)")%x%y%z%xr%yr%zr);
}

SORE_Graphics::CameraTask::CameraTask(SORE_Kernel::GameKernel* gk) : Task(gk)
{
	forward = back = left = right = false;
	cam = NULL;
	grabbed = false;
}

SORE_Graphics::CameraTask::CameraTask(Camera* camera, SORE_Kernel::GameKernel* gk) : Task(gk) 
{
	forward = back = left = right = false;
	SetCamera(camera);
	grabbed = false;
}

void SORE_Graphics::CameraTask::Pause()
{
	forward = back = left = right = false;
}

void SORE_Graphics::CameraTask::Resume()
{
	forward = back = left = right = false;
}

void SORE_Graphics::CameraTask::Frame(int elapsedTime)
{
	const static float PI_180 = M_PI/180.0f;
	GLfloat* rotation = cam->GetRotation();
	GLfloat yr = rotation[1];
	GLfloat xr = rotation[0];
	
	GLfloat x_rad, y_rad;
	x_rad = xr*PI_180;
	y_rad = yr*PI_180;
	
	float rot_const = 0.04f*elapsedTime;
	
	if(forward)
	{
		cam->Translate((GLfloat)(-rot_const * sin(y_rad)*fabs(cos(x_rad))), (GLfloat)(rot_const * sin(x_rad)), (GLfloat)(-rot_const * cos(y_rad)*fabs(cos(x_rad))));
	}
	if(back)
	{
		cam->Translate((GLfloat)(rot_const * sin(y_rad)*fabs(cos(x_rad))), (GLfloat)(-rot_const * sin(x_rad)), (GLfloat)(rot_const * cos(y_rad)*fabs(cos(x_rad))));
	}
	if(left)
	{
		cam->Translate((GLfloat)(rot_const * sin((yr-90.0)*PI_180)), (GLfloat)(0.0f), (GLfloat)(rot_const * cos((yr-90.0)*PI_180)));
	}
	if(right)
	{
		cam->Translate((GLfloat)(rot_const * sin((yr+90.0)*PI_180)), (GLfloat)(0.0f), (GLfloat)(rot_const * cos((yr+90.0)*PI_180)));
	}
}

bool SORE_Graphics::CameraTask::CameraCallback(SORE_Kernel::Event* event)
{
	const static float MOUSE_SENS = 4.0;
	switch(event->type)
	{
		case SORE_Kernel::MOUSEMOVE:
			if(grabbed)
			{
				GLfloat y = event->mouse.ymove/(GLfloat)MOUSE_SENS;
				GLfloat x = event->mouse.xmove/(GLfloat)MOUSE_SENS;
		
				cam->Rotate(-y, 0.0f, 0.0f);
				cam->Rotate(0.0f, -x, 0.0f);
			}
			break;
		case SORE_Kernel::KEYDOWN:
			switch(event->key.keySym)
			{
				case SDLK_w:
					forward = true;
					return true;
					break;
				case SDLK_a:
					left = true;
					return true;
					break;
				case SDLK_s:
					back = true;
					return true;
					break;
				case SDLK_d:
					right = true;
					return true;
					break;
				case SDLK_m:
					if(!grabbed)
					{
						SDL_WM_GrabInput(SDL_GRAB_ON);
						SDL_ShowCursor(SDL_DISABLE);
						ENGINE_LOG(SORE_Logging::LVL_DEBUG1, "Mouse grabbed");
						grabbed = !grabbed; 
						return true;
					}
					break;
				case SDLK_l:
					cam->LogPosition();
					break;
				case SDLK_u:
					if(grabbed)
					{
						SDL_WM_GrabInput(SDL_GRAB_OFF);
						SDL_ShowCursor(SDL_ENABLE);
						ENGINE_LOG(SORE_Logging::LVL_DEBUG1, "Mouse ungrabbed");
						grabbed = !grabbed;
						return true;
					}
			}
			break;
		case SORE_Kernel::KEYUP:
			switch(event->key.keySym)
			{
				case SDLK_w:
					forward = false;
					return true;
					break;
				case SDLK_a:
					left = false;
					return true;
					break;
				case SDLK_s:
					back = false;
					return true;
					break;
				case SDLK_d:
					right = false;
					return true;
					break;
			}
			break;
	}
	return false;
}

void SORE_Graphics::CameraTask::SetCamera(Camera* camera)
{
	cam = camera;
}
