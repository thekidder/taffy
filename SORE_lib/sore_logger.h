// $Id$

#ifndef  __SORE_LOGGER_H__
#define  __SORE_LOGGER_H__

#include <vector>
#include <map>
#include <cstdio>
#include <string>
#include <boost/format.hpp>

#ifndef _WIN32
#define ENGINE_LOG(lvl, format) SORE_Logging::sore_log.Log(lvl, __LINE__, __PRETTY_FUNCTION__, __FILE__, format)
#else
#define ENGINE_LOG(lvl, format) SORE_Logging::sore_log.Log(lvl, __LINE__, __FUNCTION__, __FILE__, format)
#endif

#ifdef DEBUG
#define SORE_CONSOLE_LOG
#else
#undef SORE_CONSOLE_LOG
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
	
	void AddLogLevel(int lvl, const char* name); //name should be 8 characters long
	
	class LoggerBackend
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
	
	class FileLogger : public LoggerBackend
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
	
	class ConsoleLogger : public LoggerBackend
	{
		public:
			ConsoleLogger(int lvl);
			~ConsoleLogger();
			
			void Flush();
		protected:
			void Write(log_message* log);
	};
	
	class XMLLogger : public LoggerBackend
	{
		public:
			XMLLogger(int lvl, std::string filename);
			~XMLLogger();
			
			void Flush();
		protected:
			void Write(log_message* log);
			std::string file;
			FILE* filePtr;
			std::string prevFunc;
			bool first;
			bool inFunc;
			bool ok; //file stream is good for writing
	};
	
	class Logger
	{
		public:
			Logger();
			Logger(const char* name); //create a named logger
			~Logger();
		
			void AddBackend(LoggerBackend* newLog);
			//void Log(int lvl, const char* format, ...);
			void Log(int lvl, std::string message);
			//void Log(int lvl, int line, const char* func, const char* file, const char* format, ...);
			void Log(int lvl, int line, const char* func, const char* file, std::string message, int module=MODULE_NONE);
			void Log(int lvl, int line, const char* func, const char* file, boost::format message, int module=MODULE_NONE);
			void Flush();
			
			void IgnoreModule(int module, int level);
			
			const char* GetName() const;
		protected:
			std::vector<LoggerBackend*> logs;
			std::vector<LoggerBackend*>::iterator it;
			std::vector<log_message> buffers;
			std::string logName;
			std::vector<std::pair<int,int> > ignoredModules;
	};
	extern Logger sore_log;
}

#endif /*__SORE_LOGGER_H__*/
