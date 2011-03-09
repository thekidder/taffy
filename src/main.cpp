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
    mainLog = new SORE_Logging::Logger(GetVersionDisplayName().c_str());
    std::string logName = "logs/" + GetVersionName() + "_log.xml";
    std::string logTitle = GetVersionDisplayName() + " log";
    SORE_Logging::XMLLogger fileLog(SORE_Logging::SHOW_ALL, logName, logTitle);

    mainLog->AddBackend(&fileLog);
#ifdef DEBUG
    SORE_Logging::ConsoleLogger consoleLog(SORE_Logging::SHOW_ALL);
    mainLog->AddBackend(&consoleLog);
    APP_LOG(SORE_Logging::LVL_INFO, "DEBUG BUILD");
    const char* settingsFile = "settings_debug.ini";
#else
    const char* settingsFile = "settings.ini";
#endif

    APP_LOG(SORE_Logging::LVL_INFO, GetVersionString());

    SORE_FileIO::PackageCache pc;
    pc.AddPackage("default_shaders.sdp");
    pc.AddPackage("ix_style.sdp");

    SORE_Game::GamestateManager gsm(&pc, GetVersionDisplayName(), "data/app.bmp", settingsFile);

    gsm.PushState(new DefaultState);

    int toReturn = gsm.Run();
    delete mainLog;
    return toReturn;
}

