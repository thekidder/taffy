
#include "main.h"

SORE_Logging::Logger* mainLog;
SORE_Logging::XMLLogger* fileLog;
SORE_Logging::ConsoleLogger* consoleLog;
SORE_Kernel::Task* renderer;
SORE_Kernel::Task* input;

int main(int argc, char *argv[])
{
	mainLog = new SORE_Logging::Logger("SORE_test");
	fileLog = new SORE_Logging::XMLLogger(SORE_Logging::INFO, "logs/program_log.xml");
	consoleLog = new SORE_Logging::ConsoleLogger(SORE_Logging::INFO);
	mainLog->AddBackend(fileLog);
	mainLog->AddBackend(consoleLog);
#ifdef DEBUG
	APP_LOG_S(SORE_Logging::LVL_INFO, "DEBUG BUILD");
#endif
	SORE_FileIO::InitFileIO();
	SORE_FileIO::CachePackageInfo("data_compressed.sdp");
	
	SORE_Font::InitFontSystem();
	
	SORE_Kernel::GameKernel* gk = SORE_Kernel::GameKernel::GetKernel();
	renderer = new SORE_Kernel::Renderer;
	input    = new SORE_Kernel::InputTask;
	gk->AddTask(10, renderer);
	gk->AddTask(20, input);
	
	const double maxFPS = 100.0;
	
	int ticks, lastTicks;
	
	lastTicks = SDL_GetTicks();
	
	
	bool done = false;
	
	while(!gk->quitFlag)
	{
		ticks = SDL_GetTicks();
		
		int ms = ticks - lastTicks;
		
		if(1000.0/double(ms)>maxFPS)
		{
			SDL_Delay(1);
			continue;
		}
		gk->Frame();
		lastTicks = ticks;
	}
	
	Cleanup();
	return 0;
}

void Cleanup()
{
	SORE_Kernel::GameKernel* gk = SORE_Kernel::GameKernel::GetKernel();
	gk->RemoveAllTasks();
	
	delete renderer;
	delete input;
	
	gk->Cleanup();
	
	delete mainLog;
	delete fileLog;
	delete consoleLog;
}
