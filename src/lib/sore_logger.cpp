/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <sore_defines.h>

#include <sore_allgl.h>
#include <sore_logger.h>
#include <sore_timing.h>

#include <sore_logginginit.h>

#include <cstring>
#include <string>
#include <ctime>
#include <cstdarg>
#include <cstdlib>

#define FLUSH_MESSAGES
#define SORE_CONSOLE_LOG

#ifndef DEBUG
#ifdef _WIN32
#undef SORE_CONSOLE_LOG
#endif
#endif

#define SORE_FILE_LOG

#ifdef DEBUG
#define FILE_LOG_LVL SHOW_ALL
#else
#define FILE_LOG_LVL SHOW_INFO
#endif

namespace SORE_Logging
{
	static std::map<int, const char*> lvlNames;
#ifdef SORE_FILE_LOG
	XMLLogger sore_file_logger(FILE_LOG_LVL, "logs/sore_log.xml", "SORE Engine Log");
#endif
#ifdef SORE_CONSOLE_LOG
	ConsoleLogger sore_console_logger(SHOW_ALL);
#endif

	Logger SORE_EXPORT sore_log("SORE Engine");
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
	lvlNames[LVL_DEBUG3  ] = "Debug 3 ";
#ifdef SORE_FILE_LOG
	sore_log.AddBackend(&sore_file_logger);
#endif
#ifdef SORE_CONSOLE_LOG
	sore_log.AddBackend(&sore_console_logger);
#endif
	LoggingStartMessages();
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

SORE_Logging::FileLogger::FileLogger(int lvl, std::string filename)
{
	level = lvl;
	file = filename;
	filePtr = fopen(file.c_str(), "w");
}

SORE_Logging::FileLogger::~FileLogger()
{
	fclose(filePtr);
}

void SORE_Logging::FileLogger::Write(log_message* log)
{
	std::string buffer;
	std::string levelstr;
	if(lvlNames.find(log->level)!=lvlNames.end())
	{
		levelstr = boost::str(boost::format("%-8s") % lvlNames[log->level]);
	}
	else
	{
		levelstr = boost::str(boost::format("%-8d") % log->level);
	}
	buffer = boost::str(boost::format("[%s] %s\n") % levelstr % log->buffer);
	fwrite(buffer.c_str(), sizeof(char), strlen(buffer.c_str()), filePtr);
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
	std::string buffer;
	std::string level;
	std::string name;
	if(log->logName.empty())
	{
		name = "Unnamed";
	}
	else
	{
		name = log->logName;
	}
	if(lvlNames.find(log->level)!=lvlNames.end())
	{
		level = boost::str(boost::format("%-8s") % lvlNames[log->level]);
	}
	else
	{
		level = boost::str(boost::format("%-8d") % log->level);
	}
	buffer = boost::str(boost::format("[%-12s] [%s] %s\n") % name % level % log->buffer);
	fwrite(buffer.c_str(), sizeof(char), strlen(buffer.c_str()), stdout);
}

SORE_Logging::XMLLogger::XMLLogger(int lvl, std::string filename, std::string logName)
{
	
	const char begin[] = "<?xml version=\"1.0\"?>\n<?xml-stylesheet type=\"text/xsl\" href=\"style.xsl\"?>\n<log filename=\"";
	const char end[] = "\">\n";
	std::string name_str = "<logname>"+logName+"</logname>\n";
	level = lvl;
	name = logName;
	file = filename;
	filePtr = fopen(file.c_str(), "w");
	if(filePtr==NULL || ferror(filePtr)!=0)
	{
		//don't log this failure - as XMLLogger hasn't been instantiated, the Logger probably hasn't been either
		ok = false;
		//ENGINE_LOG(SORE_Logging::LVL_ERROR, "Failed to open log file for writing");
	}
	else
		ok = true;
	if(ok)
	{
		fwrite(begin, sizeof(char), strlen(begin), filePtr);
		fwrite(filename.c_str(), sizeof(char), strlen(filename.c_str()), filePtr);
		fwrite(end, sizeof(char), strlen(end), filePtr);
		fwrite(name_str.c_str(), sizeof(char), strlen(name_str.c_str()), filePtr);
	}
	first = true;
	inFunc = false;
}

SORE_Logging::XMLLogger::~XMLLogger()
{
	if(!first && ok)
		fwrite("</function>\n", sizeof(char), 12, filePtr);

	const char end[] = "</log>\n";
	if(ok)
	{
		fwrite(end, sizeof(char), strlen(end), filePtr);
		fclose(filePtr);
	}
}

void SORE_Logging::XMLLogger::Write(log_message* log)
{
	if(!ok) return;
	std::string buffer;
	std::string levelstr;
	std::string levelint;
	
	if(!first && ((log->func==NULL && !prevFunc.empty() ) || (!(log->func==NULL && prevFunc.empty()) && prevFunc!=log->func)))
	{
		buffer += "</function>\n";
	}
	if(log->func!=NULL && prevFunc!=log->func)
	{
		std::string func = log->func;
		SanitizeXML(func);
		
		buffer += boost::str(boost::format("<function>\n\t<name>%s</name>\n") % func.c_str());
	}
	else if(log->func==NULL && (!prevFunc.empty() || first))
	{
		buffer += "<function>\n\t<name></name>\n";
	}
	first = false;
	if(log->func!=NULL)
	{
		prevFunc = log->func;
	}
		
	if(lvlNames.find(log->level)!=lvlNames.end())
	{
		levelstr = lvlNames[log->level];
	}
	
	levelint = boost::str(boost::format("%d") % log->level);
	const char begin[] = "\t<message>\n";
	const char end[]   = "\t</message>\n";
	buffer += begin;
	buffer += "\t\t<time>";
	buffer += boost::str(boost::format("+%.1f ms") % double(SORE_Timing::GetGlobalTicks()/10.0));
	buffer += "</time>\n\t\t<level>";
	buffer += levelint;
	buffer += "</level>\n\t\t<levelstr>";
	buffer += levelstr;
	buffer += "</levelstr>\n";
	buffer += boost::str(boost::format("\t\t<line>%d</line>\n") % log->line);
	buffer += boost::str(boost::format("\t\t<file>%s</file>\n") % log->file);
	
	SanitizeXML(log->buffer);
	
	buffer += boost::str(boost::format("\t\t<data>%s</data>\n") % log->buffer);
	buffer += end;

	fwrite(buffer.c_str(), sizeof(char), strlen(buffer.c_str()), filePtr);
}

void SORE_Logging::XMLLogger::SanitizeXML(std::string& str)
{
	size_t pos = 0;
	while((pos=str.find("&",pos))!=std::string::npos)
	{
		str.replace(pos, 1, "&amp;");
		pos++;
	}
	pos = 0;
	while((pos=str.find("<",pos))!=std::string::npos)
	{
		str.replace(pos, 1, "&lt;");
		pos++;
	}
	pos = 0;
	while((pos=str.find(">",pos))!=std::string::npos)
	{
		str.replace(pos, 1, "&gt;");
		pos++;
	}
}

void SORE_Logging::XMLLogger::Flush()
{
	if(ok)
		fflush(filePtr);
}

SORE_Logging::Logger::Logger()
{
	buffers.clear();
	Log(LVL_INFO, "Unnamed log started");
	logName = "";
}

SORE_Logging::Logger::Logger(const char* name) : logName(name)
{
	buffers.clear();
	Log(LVL_INFO, boost::str(boost::format("%s log started") % logName));
}

SORE_Logging::Logger::~Logger()
{
	if(logName.empty())
		Log(LVL_INFO, "Unnamed log terminated");
	else
		Log(LVL_INFO, boost::str(boost::format("%s log terminated") % logName));
	Flush();
}

void SORE_Logging::Logger::AddBackend(LoggerBackend* newLog)
{
	logs.push_back(newLog);
}

void SORE_Logging::Logger::RemoveAllBackends()
{
	logs.clear();
}

void SORE_Logging::Logger::Log(int lvl, std::string message)
{
	log_message temp;
	temp.level = lvl;
	temp.line = 0;
	temp.func = 0;
	temp.file = 0;
	temp.time = time(NULL);
	temp.logName = logName;
	temp.module = MODULE_NONE;
	if(lvlNames.size()==0)
		InitLogging();
	temp.buffer = message;
	for(it=logs.begin();it<logs.end();it++)
	{
		for(size_t i=0;i<buffers.size();i++)
			(*it)->Log(&buffers[i]);
	}
	if(logs.size()>0) buffers.clear();
	if(lvl == LVL_ERROR) Flush();
#ifdef FLUSH_MESSAGES
	Flush();
#endif
}

void SORE_Logging::Logger::Log(int lvl, int line, const char* func, const char* file, std::string message, int module)
{
	std::vector<std::pair<int,int> >::iterator lit;
	for(lit=ignoredModules.begin();lit!=ignoredModules.end();lit++)
	{
		if(lit->first==module)
			if(lit->second & lvl)
				return;
	}
	log_message temp;
	temp.level = lvl;
	temp.line = line;
	temp.func = func;
	temp.file = file;
	temp.time = time(NULL);
	temp.logName = logName;
	temp.module = module;
	if(lvlNames.size()==0)
		InitLogging();
	temp.buffer = message;
	buffers.push_back(temp);
	for(it=logs.begin();it!=logs.end();it++)
	{
		for(size_t i=0;i<buffers.size();i++)
			(*it)->Log(&buffers[i]);
	}
	if(logs.size()>0) buffers.clear();
	if(lvl == LVL_ERROR) Flush();
#ifdef FLUSH_MESSAGES
	Flush();
#endif
}

void SORE_Logging::Logger::Log(int lvl, int line, const char* func, const char* file, boost::format message, int module)
{
	std::string msg = boost::str(message);
	Log(lvl, line, func, file, msg, module);
}

void SORE_Logging::Logger::Flush()
{
	for(it=logs.begin();it<logs.end();it++)
	{
		for(size_t i=0;i<buffers.size();i++)
			(*it)->Log(&buffers[i]);
		(*it)->Flush();
	}
	if(logs.size()>0) buffers.clear();
}

void SORE_Logging::Logger::SetLevel(int lvl)
{
	for(it=logs.begin();it<logs.end();it++)
	{
		(*it)->SetLevel(lvl);
	}
}

void SORE_Logging::Logger::IgnoreModule(int module, int level)
{
	ignoredModules.push_back(std::pair<int,int>(module,level));
}

const char* SORE_Logging::Logger::GetName() const
{
	return logName.c_str();
}
