//
// C++ Implementation: sore_test
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "main.h"
#include <sstream>
#include <string>

SORE_Logging::Logger* mainLog;
SORE_Logging::XMLLogger* fileLog;
SORE_Logging::ConsoleLogger* consoleLog;
SORE_Graphics::Camera cam;

SORE_Graphics::TerrainGraph* tg;

bool OptionCallback(SORE_Kernel::Event* event);
bool BallListen(SORE_Kernel::Event* event);

//std::vector<PhysicsBall> balls;

int main(int argc, char *argv[])
{
	mainLog = new SORE_Logging::Logger("SORE_test");
	fileLog = new SORE_Logging::XMLLogger(SORE_Logging::SHOW_INFO, "logs/program_log.xml", "Snake Log");
	consoleLog = new SORE_Logging::ConsoleLogger(SORE_Logging::SHOW_ALL);
	mainLog->AddBackend(fileLog);
	mainLog->AddBackend(consoleLog);
#ifdef DEBUG
	APP_LOG_S(SORE_Logging::LVL_INFO, "DEBUG BUILD");
#endif
	SORE_FileIO::InitFileIO();
	SORE_FileIO::CachePackageInfo("data_compressed.sdp");
	
	SORE_Font::InitFontSystem();
	
	
	
	cam.SetRotationUpperLimit(AXIS_X,  90.0f);
	cam.SetRotationLowerLimit(AXIS_X, -90.0f);
	
	cam.Rotate(0.0f, -120.0f, 0.0f);
	cam.Translate(0.0f, 2.0f, 0.0f);
	
	
	SORE_Kernel::GameKernel gk;
	PhysicsTask physTask(&gk);
	SORE_Kernel::Renderer renderer(&gk);
	SORE_Kernel::InputTask input(&gk);
	
	SORE_Graphics::CameraTask camTask(&cam, &gk);
	
	char version[4];
	
	strncpy(version, (char*)glGetString(GL_VERSION),3);
	version[3] = '\0';
	int major = int(version[0])-48;
	int minor = int(version[2])-48;
	if(major<2)
	{
		APP_LOG_S(SORE_Logging::LVL_CRITICAL, "OpenGL Version is less than 2.0. Aborting.");

		gk.quitFlag = true;
	}
		
	tg = new SORE_Graphics::TerrainGraph(70, 70, &gk);
	tg->WritePGM("map.pgm");
	
	renderer.SetSceneGraph(tg);
	renderer.SetCamera(&cam);
	
	tg->AddPhysicsEngine(&physTask);
		
	PhysicsBall newBall(100.0, 12.0, 100.0);
	physTask.AddObject(newBall);
	
	PhysicsBall newBall2(100.0, 14.0, 100.0);
	physTask.AddObject(newBall2);
	
	input.AddListener(SORE_Kernel::KEYDOWN | SORE_Kernel::KEYUP | SORE_Kernel::MOUSEMOVE, SORE_Kernel::MakeFunctor<SORE_Graphics::CameraTask>(&camTask, &SORE_Graphics::CameraTask::CameraCallback));
	input.AddListener(SORE_Kernel::KEYDOWN, SORE_Kernel::MakeFunctor<PhysicsTask>(&physTask, &PhysicsTask::PhysicsCallback));
	input.AddListener(SORE_Kernel::RESIZE , SORE_Kernel::MakeFunctor<SORE_Kernel::Renderer>(&renderer, &SORE_Kernel::Renderer::OnResize));
	//input.AddListener(SORE_Kernel::RESIZE , SORE_Kernel::MakeFunctor(SORE_Graphics::OnResize));
	input.AddListener(SORE_Kernel::KEYDOWN , SORE_Kernel::MakeFunctor(BallListen));
	input.AddListener(SORE_Kernel::KEYDOWN | SORE_Kernel::KEYUP, SORE_Kernel::MakeFunctor(OptionCallback));
	input.AddListener(SORE_Kernel::KEYDOWN | SORE_Kernel::KEYUP, SORE_Kernel::MakeFunctor<SORE_Graphics::TerrainGraph>(tg, &SORE_Graphics::TerrainGraph::LightMoveCallback));
	gk.AddTask(20, &renderer);
	gk.AddTask(10, &input);
	gk.AddTask(30, &camTask);
	gk.AddTask(40, tg);
	gk.AddConstTask(50, 1, &physTask);
	
	const double maxFPS = 500.0;
	
	int ticks, lastTicks;
	
	lastTicks = SDL_GetTicks();
	
	bool done = false;
	
	int loop = 1;
	
	while(!gk.quitFlag)
	{
		SORE_Profiler::Sample mainLoop("topmost");
		ticks = SDL_GetTicks();
		
		int ms = ticks - lastTicks;
		
		if(1000.0/double(ms)>maxFPS)
		{
			SDL_Delay(1);
			continue;
		}
		gk.Frame();

		lastTicks = ticks;
		//if(loop%100==0)
		//{
			//loop = 0;
			//SORE_Profiler::Sample::DisplayAvgTime(SORE_Profiler::Sample::GetSampleByName("topmost"));
		//}
		//loop++;
	}
	SORE_Profiler::Sample::DisplayAvgTime(SORE_Profiler::Sample::GetSampleByName("topmost"));
	SORE_Profiler::Sample::DisplayAvgTime(SORE_Profiler::Sample::GetSampleByName("physics"));
	SORE_Profiler::Sample::DisplayAvgTime(SORE_Profiler::Sample::GetSampleByName("graphics"));

	gk.RemoveAllTasks();
	//gk.Cleanup();
	
	delete mainLog;
	delete fileLog;
	delete consoleLog;
	delete tg;
	return 0;
}

void Cleanup()
{
	
}

bool OptionCallback(SORE_Kernel::Event* event)
{
	if(event->type == SORE_Kernel::KEYDOWN)
	{
		if(event->key.keySym==SDLK_x)
		{
			tg->ToggleWireframe();
			APP_LOG_S(SORE_Logging::LVL_DEBUG2, "Toggled wireframe");
			return true;
		}
		else if(event->key.keySym==SDLK_n)
		{
			tg->ToggleNormals();
			APP_LOG_S(SORE_Logging::LVL_DEBUG2, "Toggled normals");
			return true;
		}
		else if(event->key.keySym==SDLK_c)
		{
			tg->ToggleHeightmapColoring();
			APP_LOG_S(SORE_Logging::LVL_DEBUG2, "Toggled coloring");
			return true;
		}
		else if(event->key.keySym==SDLK_l)
		{
			tg->ToggleLighting();
			APP_LOG_S(SORE_Logging::LVL_DEBUG2, "Toggled lighting modes");
			return true;
		}
	}
	return false;
}

void RenderSphere(float r, int div)
{
	glMatrixMode(GL_MODELVIEW);
	glBegin(GL_TRIANGLES);
	
	for(int i=0;i<1;i++)
	{
		glLoadIdentity();
		glVertex3f(0.0f,-r,0.0f);
		//glPushMatrix();
		glRotatef(360.0f/div*i, 0.0f,1.0f,0.0f);
		glVertex3f(1.0f/div, 1.0f/div, 1.0f/div);
		glRotatef(45.0f, 0.0f,0.0f,1.0f);
		glVertex3f(1.0f/div, 1.0f/div, 1.0f/div);
		//glPopMatrix();
	}
	
	//glVertex3f(0.0f,-r,0.0f);
	//glVertex3f(1.0f/div, 0.0f,0.0f);
	//glVertex3f(1.0f/-div,0.0f,0.0f);
	
	for(int i=0;i<div-2;i++)
	{
		
	}
	
	for(int i=0;i<div;i++)
	{
		
	}
	
	glEnd();
}

void AddBall()
{
	//PhysicsBall newBall(100.0, 14.0, 100.0);
	//physTask.AddObject(newBall);
}

bool BallListen(SORE_Kernel::Event* event)
{
	if(event->type==SORE_Kernel::KEYDOWN)
	{
		if(event->key.keySym==SDLK_k)
		{
			AddBall();
			return true;
		}
		return false;
	}
}
