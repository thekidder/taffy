//
// sore_test generic header file
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef  __MAIN_H__
#define  __MAIN_H__

#include <sore.h>
#include "terrain_graph.h"

#define APP_LOG(lvl, format, ...) mainLog->Log(lvl, __LINE__, __PRETTY_FUNCTION__, __FILE__, format, __VA_ARGS__) 
#define APP_LOG_S(lvl, format) mainLog->Log(lvl, __LINE__, __PRETTY_FUNCTION__, __FILE__, format)

//Functions
void Cleanup();

extern SORE_Logging::Logger* mainLog;
extern SORE_Logging::XMLLogger* fileLog;
extern SORE_Logging::ConsoleLogger* consoleLog;

#endif //__MAIN_H__
