
#include "main.h"

SORE_Logging::Logger* mainLog;
SORE_Logging::XMLLogger* fileLog;
SORE_Logging::ConsoleLogger* consoleLog;
SORE_Kernel::Task* renderer;
SORE_Kernel::Task* input;

int main(int argc, char *argv[])
{
	mainLog = new SORE_Logging::Logger;
	fileLog = new SORE_Logging::XMLLogger(SORE_Logging::INFO, "logs/program_log.xml");
	consoleLog = new SORE_Logging::ConsoleLogger(SORE_Logging::INFO);
	mainLog->AddBackend(fileLog);
	mainLog->AddBackend(consoleLog);
#ifdef DEBUG
	APP_LOG_S(SORE_Logging::LVL_INFO, "DEBUG BUILD");
#endif
	SORE_FileIO::Init();
	SORE_FileIO::CachePackageInfo("data_compressed.sdp");
	
	SORE_Kernel::GameKernel* gk = SORE_Kernel::GameKernel::GetKernel();
	renderer = new SORE_Kernel::Renderer;
	input    = new SORE_Kernel::InputTask;
	gk->AddTask(10, renderer);
	gk->AddTask(20, input);
	Cleanup();
	return 0;
}

void Cleanup()
{
	delete renderer;
	delete input;
	
	delete mainLog;
	delete fileLog;
	delete consoleLog;
}
