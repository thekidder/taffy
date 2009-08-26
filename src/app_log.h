// $Id$

#ifndef  APP_LOG_H
#define  APP_LOG_H

#include <sore_logger.h>

#ifndef _WIN32
#define APP_LOG(lvl, format) mainLog->Log(lvl, __LINE__, __PRETTY_FUNCTION__, __FILE__, format)
#else
#define APP_LOG(lvl, format) mainLog->Log(lvl, __LINE__, __FUNCTION__, __FILE__, format)
#endif

extern SORE_Logging::Logger* mainLog;

void PrintVersionInfo();

#endif //APP_LOG_H
