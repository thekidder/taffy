// $Id$

#include <sore_logger.h>
#include "app_log.h"

#include <sore_gamekernel.h>
#include <sore_gamestate.h>
#include <sore_gamestate_manager.h>

#include "state_default.h"

SORE_Logging::Logger* mainLog;

int main(int argc, char** argv)
{
	srand(static_cast<unsigned int>(time(0)));
	mainLog = new SORE_Logging::Logger("program_name");
	SORE_Logging::XMLLogger fileLog(SORE_Logging::SHOW_ALL, "logs/program_log.xml", "program_name log");
#ifdef DEBUG
	SORE_Logging::ConsoleLogger consoleLog(SORE_Logging::SHOW_ALL);
#endif

	mainLog->AddBackend(&fileLog);

#ifdef DEBUG
	mainLog->AddBackend(&consoleLog);
	APP_LOG(SORE_Logging::LVL_INFO, "DEBUG BUILD");
#endif

	PrintVersionInfo();
	
	//we need a gamekernel to run all the tasks in
	SORE_Kernel::GameKernel gk;
	SORE_FileIO::InitFileIO(&gk);
    SORE_FileIO::CachePackageInfo("ix_style.sdp");

	SORE_Game::GamestateManager gsm(gk);

	gsm.PushState(new DefaultState);

	int toReturn = gsm.Run();
	delete mainLog;
	return toReturn;
}

