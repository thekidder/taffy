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

#ifndef  SORE_LOGGER_H
#define  SORE_LOGGER_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <vector>
#include <map>
#include <cstdio>
#include <string>

#include <boost/format.hpp>

#include <sore_dll.h>

#ifndef _WIN32
#define ENGINE_LOG(lvl, format) SORE_Logging::sore_log.Log(lvl, __LINE__, __PRETTY_FUNCTION__, __FILE__, format)
#define ENGINE_LOG_M(lvl, format, module) SORE_Logging::sore_log.Log(lvl, __LINE__, __PRETTY_FUNCTION__, __FILE__, format, module)
#else
#define ENGINE_LOG(lvl, format) SORE_Logging::sore_log.Log(lvl, __LINE__, __FUNCTION__, __FILE__, format)
#define ENGINE_LOG_M(lvl, format, module) SORE_Logging::sore_log.Log(lvl, __LINE__, __FUNCTION__, __FILE__, format, module)
#endif

namespace SORE_Logging
{
	const int LVL_CRITICAL = 0x01;
	const int LVL_ERROR    = 0x02;
	const int LVL_WARNING  = 0x04;
	const int LVL_INFO     = 0x08;
	const int LVL_DEBUG1   = 0x10;
	const int LVL_DEBUG2   = 0x20;
	const int LVL_DEBUG3   = 0x40;
	
	const int SHOW_CRITICAL     = LVL_CRITICAL;
	const int SHOW_ERROR        = SHOW_CRITICAL | LVL_ERROR;
	const int SHOW_WARNING      = SHOW_ERROR    | LVL_WARNING;
	const int SHOW_INFO         = SHOW_WARNING  | LVL_INFO;
	const int SHOW_ALL          = SHOW_INFO     | LVL_DEBUG1 | LVL_DEBUG2 | LVL_DEBUG3;
	
	const int MODULE_NONE  = 0;
		
	struct log_message
	{
		int level;
		int line;
		const char* func;
		const char* file;
		std::string buffer;
		time_t time;
		std::string logName;
		int module;
	};
	
	void SORE_EXPORT AddLogLevel(int lvl, const char* name); //name should be 8 characters long
	
	class SORE_EXPORT LoggerBackend
	{
		public:
			virtual ~LoggerBackend() {}
			virtual void Flush() = 0;
			void Log(log_message* log);
			void SetLevel(int lvl);
		protected:
			virtual void Write(log_message* log) = 0;
			int level;
	};
	
	class SORE_EXPORT FileLogger : public LoggerBackend
	{
		public:
			FileLogger(int lvl, std::string filename);
			~FileLogger();
			
			void Flush();
		protected:
			void Write(log_message* log);
			std::string file;
			FILE* filePtr;
	};
	
	class SORE_EXPORT ConsoleLogger : public LoggerBackend
	{
		public:
			ConsoleLogger(int lvl);
			~ConsoleLogger();
			
			void Flush();
		protected:
			void Write(log_message* log);
	};
	
	class SORE_EXPORT XMLLogger : public LoggerBackend
	{
		public:
			XMLLogger(int lvl, std::string filename, std::string logName);
			~XMLLogger();
			
			void Flush();
		protected:
			void Write(log_message* log);
			void SanitizeXML(std::string& str);
			std::string file, name;
			FILE* filePtr;
			std::string prevFunc;
			bool first;
			bool inFunc;
			bool ok; //file stream is good for writing
	};
	
	class SORE_EXPORT Logger
	{
		public:
			Logger();
			Logger(const char* name); //create a named logger
			~Logger();
		
			void AddBackend(LoggerBackend* newLog);
			void RemoveAllBackends();
			//void Log(int lvl, const char* format, ...);
			void Log(int lvl, std::string message);
			//void Log(int lvl, int line, const char* func, const char* file, const char* format, ...);
			void Log(int lvl, int line, const char* func, const char* file, std::string message, int module=MODULE_NONE);
			void Log(int lvl, int line, const char* func, const char* file, boost::format message, int module=MODULE_NONE);
			void Flush();

			void SetLevel(int lvl); //set level of all backends
			
			void IgnoreModule(int module, int level);
			
			const char* GetName() const;
		private:
			std::vector<LoggerBackend*> logs;
			std::vector<LoggerBackend*>::iterator it;
			std::vector<log_message> buffers;
			std::string logName;
			std::vector<std::pair<int,int> > ignoredModules;
	};
	extern SORE_EXPORT Logger sore_log;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
