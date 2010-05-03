/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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

#include "sore_dll.h"

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
