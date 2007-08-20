
#include "sore_logger.h"
#include <string.h>
#include <ctime>
#include <cstdarg>

namespace SORE_Logging
{
	static std::map<int, const char*> lvlNames;
}

void SORE_Logging::InitLogging()
{
	lvlNames.clear();
	lvlNames[LVL_CRITICAL] = "Critical";
	lvlNames[LVL_ERROR   ] = "Error   ";
	lvlNames[LVL_WARNING ] = "Warning ";
	lvlNames[LVL_INFO    ] = "Info    ";
	lvlNames[LVL_DEBUG1  ] = "Debug 1 ";
	lvlNames[LVL_DEBUG2  ] = "Debug 2 ";
	
}

void SORE_Logging::AddLogLevel(int lvl, const char* name)
{
	if(lvlNames.find(lvl)==lvlNames.end())
	{
		lvlNames[lvl] = name;
	}
}

SORE_Logging::FileLogger::FileLogger(int lvl, const char* filename)
{
	level = lvl;
	strncpy(file, filename, 255);
	filePtr = fopen(file, "w");
}

SORE_Logging::FileLogger::~FileLogger()
{
	fclose(filePtr);
}

void SORE_Logging::FileLogger::SetLevel(int lvl)
{
	level = lvl;
}

void SORE_Logging::FileLogger::Write(int lvl, const char* string)
{
	if(level & lvl)
		fputs(string, filePtr);
}

void SORE_Logging::FileLogger::Flush()
{
	fflush(filePtr);
}

SORE_Logging::Logger::Logger()
{
	buffers.clear();
	time_t curr = time(NULL);
	tm* curr_local;
	curr_local = localtime(&curr);
	Log(LVL_INFO, "Program log started at %s", asctime(curr_local));
}

void SORE_Logging::Logger::AddBackend(LoggerBackend* newLog)
{
	logs.push_back(newLog);
}

void SORE_Logging::Logger::Log(int lvl, const char* format, ...)
{
	log_buffer temp;
	temp.level = lvl;
	strcpy(temp.buffer,  "[");
	if(SORE_Logging::lvlNames.find(lvl)==lvlNames.end())
	{
		sprintf(temp.buffer+strlen(temp.buffer), "%8d", lvl);
	}
	else
	{
		strncat(temp.buffer, lvlNames[lvl], BUFFER_LEN-strlen(temp.buffer)-1);
	}
	strncat(temp.buffer, "] ", BUFFER_LEN-strlen(temp.buffer)-1);
	va_list args;
	va_start (args, format);
	vsprintf (temp.buffer+strlen(temp.buffer),format, args);
	va_end (args);
	strncat(temp.buffer, "\n", BUFFER_LEN-strlen(temp.buffer)-1);
	buffers.push_back(temp);
	for(it=logs.begin();it<logs.end();it++)
	{
		for(int i=0;i<buffers.size();i++)
			(*it)->Write(buffers[i].level, buffers[i].buffer);
	}
	if(logs.size()>0) buffers.clear();
}

void SORE_Logging::Logger::Flush()
{
	for(it=logs.begin();it<logs.end();it++)
	{
		for(int i=0;i<buffers.size();i++)
			(*it)->Write(buffers[i].level, buffers[i].buffer);
		(*it)->Flush();
	}
	if(logs.size()>0) buffers.clear();
}
