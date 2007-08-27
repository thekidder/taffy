
#include "sore_logger.h"
#include <cstring>
#include <ctime>
#include <cstdarg>
#include <cstdlib>

namespace SORE_Logging
{
	static std::map<int, const char*> lvlNames;
	XMLLogger sore_file_logger(INFO | LVL_DEBUG1, "logs/sore_log.xml");
#ifdef SORE_CONSOLE_LOG
	ConsoleLogger sore_console_logger(ALL);
#endif
	Logger sore_log("SORE Engine");
	
	void InitLogging();
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
	sore_log.AddBackend(&sore_file_logger);
	sore_log.AddBackend(&sore_console_logger);
}

void SORE_Logging::AddLogLevel(int lvl, const char* name)
{
	if(lvlNames.find(lvl)==lvlNames.end())
	{
		lvlNames[lvl] = name;
	}
}

void SORE_Logging::LoggerBackend::SetLevel(int lvl)
{
	level = lvl;
}

void SORE_Logging::LoggerBackend::Log(log_message* log)
{
	if(level & (log->level))
		Write(log);
}

SORE_Logging::FileLogger::FileLogger(int lvl, const char* filename)
{
	level = lvl;
	strncpy(file, filename, 255);
	file[255] = '\0';
	filePtr = fopen(file, "w");
}

SORE_Logging::FileLogger::~FileLogger()
{
	fclose(filePtr);
}

void SORE_Logging::FileLogger::Write(log_message* log)
{
	static char buffer[2048];
	char levelstr[9];
	if(lvlNames.find(log->level)!=lvlNames.end())
	{
		strncpy(levelstr, lvlNames[log->level], 8);
		levelstr[8] = '\0';
	}
	else
	{
		sprintf(levelstr, "%-8d", log->level);
	}
	sprintf(buffer, "[%s] %s\n", levelstr, log->buffer);
	fwrite(buffer, sizeof(char), strlen(buffer), filePtr);
}

void SORE_Logging::FileLogger::Flush()
{
	fflush(filePtr);
}

SORE_Logging::ConsoleLogger::ConsoleLogger(int lvl)
{
	level = lvl;
}

SORE_Logging::ConsoleLogger::~ConsoleLogger() {}

void SORE_Logging::ConsoleLogger::Flush()
{
	fflush(stdout);
}

void SORE_Logging::ConsoleLogger::Write(log_message* log)
{
	static char buffer[2048];
	char levelstr[9];
	char name[13];
	if(strlen(log->logName)==0)
	{
		strcpy(name, "Unnamed");
	}
	else
	{
		strncpy(name, log->logName, 12);
		name[12] = '\0';
	}
	if(lvlNames.find(log->level)!=lvlNames.end())
	{
		strncpy(levelstr, lvlNames[log->level], 8);
		levelstr[8] = '\0';
	}
	else
	{
		sprintf(levelstr, "%-8d", log->level);
	}
	sprintf(buffer, "[%-12s] [%s] %s\n", name, levelstr, log->buffer);
	fwrite(buffer, sizeof(char), strlen(buffer), stdout);
}

SORE_Logging::XMLLogger::XMLLogger(int lvl, const char* filename)
{
	const char begin[] = "<?xml version=\"1.0\"?>\n<?xml-stylesheet type=\"text/xsl\" href=\"style.xsl\"?>\n<log filename=\"";
	const char end[] = "\">\n";
	level = lvl;
	strncpy(file, filename, 255);
	file[255] = '\0';
	filePtr = fopen(file, "w");
	fwrite(begin, sizeof(char), strlen(begin), filePtr);
	fwrite(filename, sizeof(char), strlen(filename), filePtr);
	fwrite(end, sizeof(char), strlen(end), filePtr);
	prevFunc[0] = '\0';
	first = true;
}

SORE_Logging::XMLLogger::~XMLLogger()
{
	if(!first)
		fwrite("</function>\n", sizeof(char), 12, filePtr);

	const char end[] = "</log>\n";
	fwrite(end, sizeof(char), strlen(end), filePtr);
	fclose(filePtr);
}

void SORE_Logging::XMLLogger::Write(log_message* log)
{
	static char buffer[2048];
	char levelstr[9];
	char levelint[9];
	
	if(!first && ((log->func==NULL && strlen(prevFunc)>0) || strcmp(prevFunc, log->func)!=0))
	{
		sprintf(buffer, "</function>\n");
		fwrite(buffer, sizeof(char), strlen(buffer), filePtr);
	}
	if(log->func!=NULL && strcmp(prevFunc, log->func)!=0)
	{
		sprintf(buffer, "<function>\n\t<name>%s</name>\n", log->func);
		fwrite(buffer, sizeof(char), strlen(buffer), filePtr);
	}
	else if(log->func==NULL && (strlen(prevFunc)>0 || first))
	{
		sprintf(buffer, "<function>\n\t<name></name>\n");
		fwrite(buffer, sizeof(char), strlen(buffer), filePtr);
	}
	first = false;
	if(log->func==NULL)
		prevFunc[0] = '\0';
	else
	{
		strncpy(prevFunc, log->func, 255);
		prevFunc[255] = '\0';
	}
		
	if(lvlNames.find(log->level)!=lvlNames.end())
	{
		strncpy(levelstr, lvlNames[log->level], 8);
		levelstr[8] = '\0';
	}
	else
	{
		levelstr[0]='\0';
	}
	sprintf(levelint, "%d", log->level);
	const char begin[] = "\t<message>\n";
	const char end[]   = "\t</message>\n";
	fwrite(begin, sizeof(char), strlen(begin), filePtr);
	tm* currtime;
	currtime = localtime(&(log->time));
	strftime(buffer, 127, "%X", currtime);
	fwrite("\t\t<time>", sizeof(char), 8, filePtr);
	fwrite(buffer, sizeof(char), strlen(buffer), filePtr);
	fwrite("</time>\n", sizeof(char), 8, filePtr);
	fwrite("\t\t<level>", sizeof(char), 9, filePtr);
	fwrite(levelint, sizeof(char), strlen(levelint), filePtr);
	fwrite("</level>\n", sizeof(char), 9, filePtr);
	fwrite("\t\t<levelstr>", sizeof(char), 12, filePtr);
	fwrite(levelstr, sizeof(char), strlen(levelstr), filePtr);
	fwrite("</levelstr>\n", sizeof(char), 12, filePtr);
	sprintf(buffer, "\t\t<line>%d</line>\n", log->line);
	fwrite(buffer, sizeof(char), strlen(buffer), filePtr);
	sprintf(buffer, "\t\t<file>%s</file>\n", log->file);
	fwrite(buffer, sizeof(char), strlen(buffer), filePtr);
	sprintf(buffer, "\t\t<data>%s</data>\n", log->buffer);
	fwrite(buffer, sizeof(char), strlen(buffer), filePtr);
	
	fwrite(end, sizeof(char), strlen(end), filePtr);
}

void SORE_Logging::XMLLogger::Flush()
{
	fflush(filePtr);
}

SORE_Logging::Logger::Logger()
{
	buffers.clear();
	Log(LVL_INFO, "Unnamed log started");
	logName[0] = '\0';
}

SORE_Logging::Logger::Logger(const char* name)
{
	buffers.clear();
	strncpy(logName, name, 31);
	logName[31] = '\0';
	Log(LVL_INFO, "%s log started", logName);
}

SORE_Logging::Logger::~Logger()
{
	if(strlen(logName)==0)
		Log(LVL_INFO, "Unnamed log terminated");
	else
		Log(LVL_INFO, "%s log terminated", logName);
	Flush();
}

void SORE_Logging::Logger::AddBackend(LoggerBackend* newLog)
{
	logs.push_back(newLog);
}

void SORE_Logging::Logger::Log(int lvl, const char* format, ...)
{
	log_message temp;
	temp.level = lvl;
	temp.line = 0;
	temp.func = 0;
	temp.file = 0;
	temp.time = time(NULL);
	temp.logName = logName;
	if(lvlNames.size()==0)
		InitLogging();
	va_list args;
	va_start (args, format);
	vsprintf (temp.buffer,format, args);
	va_end (args);
	buffers.push_back(temp);
	for(it=logs.begin();it<logs.end();it++)
	{
		for(int i=0;i<buffers.size();i++)
			(*it)->Log(&buffers[i]);
	}
	if(logs.size()>0) buffers.clear();
}

void SORE_Logging::Logger::Log(int lvl, int line, const char* func, const char* file, const char* format, ...)
{
	log_message temp;
	temp.level = lvl;
	temp.line = line;
	temp.func = func;
	temp.file = file;
	temp.time = time(NULL);
	temp.logName = logName;
	if(lvlNames.size()==0)
		InitLogging();
	va_list args;
	va_start (args, format);
	vsprintf (temp.buffer,format, args);
	va_end (args);
	buffers.push_back(temp);
	for(it=logs.begin();it<logs.end();it++)
	{
		for(int i=0;i<buffers.size();i++)
			(*it)->Log(&buffers[i]);
	}
	if(logs.size()>0) buffers.clear();
}

void SORE_Logging::Logger::Flush()
{
	for(it=logs.begin();it<logs.end();it++)
	{
		for(int i=0;i<buffers.size();i++)
			(*it)->Log(&buffers[i]);
		(*it)->Flush();
	}
	if(logs.size()>0) buffers.clear();
}

const char* SORE_Logging::Logger::GetName() const
{
	return logName;
}