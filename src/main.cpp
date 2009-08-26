// $Id$

#include <sore_logger.h>
#include "app_log.h"

#include <sore_gamekernel.h>
#include <sore_gamestate.h>
#include <sore_gamestate_manager.h>
#include <sore_input.h>
#include <sore_screen.h>
#include <sore_timing.h>
#include <gui/sore_button.h>
#include <gui/sore_framewindow.h>
#include <gui/sore_topwidget.h>
#include <gui/sore_textwidget.h>
#include <sore_renderer2d.h>

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

