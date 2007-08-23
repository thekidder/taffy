
//#include "fileio.h"
#include <vector>
#include <map>
#include <cstdio>

//#define LOG(lvl, format, ...) SORE_Logging::sore_log.Log(lvl, format, __VA_ARGS__)
//#define LOG_S(lvl, format) SORE_Logging::sore_log.Log(lvl, format)

#define LOG(lvl, format, ...) SORE_Logging::sore_log.Log(lvl, __LINE__, __PRETTY_FUNCTION__, __FILE__, format, __VA_ARGS__) 
#define LOG_S(lvl, format) SORE_Logging::sore_log.Log(lvl, __LINE__, __PRETTY_FUNCTION__, __FILE__, format)

namespace SORE_Logging
{
	const int LVL_CRITICAL = 0x01;
	const int LVL_ERROR    = 0x02;
	const int LVL_WARNING  = 0x04;
	const int LVL_INFO     = 0x08;
	const int LVL_DEBUG1   = 0x10;
	const int LVL_DEBUG2   = 0x20;
	
	const int CRITICAL     = LVL_CRITICAL;
	const int ERROR        = CRITICAL | LVL_ERROR;
	const int WARNING      = ERROR    | LVL_WARNING;
	const int INFO         = WARNING  | LVL_INFO;
	const int ALL          = INFO     | LVL_DEBUG1 | LVL_DEBUG2;
	
	const int BUFFER_LEN = 2048;
	
	struct log_message
	{
		int level;
		int line;
		const char* func;
		const char* file;
		char buffer[BUFFER_LEN];
		time_t time;
	};
	
	/*struct log_buffer
	{
		int level;
		char buffer[BUFFER_LEN];
	};*/
	
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
			FileLogger(int lvl, const char* filename);
			~FileLogger();
			
			void Flush();
		protected:
			void Write(log_message* log);
			char file[256];
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
			XMLLogger(int lvl, const char* filename);
			~XMLLogger();
			
			void Flush();
		protected:
			void Write(log_message* log);
			char file[256];
			FILE* filePtr;
			char prevFunc[256];
			bool first;
	};
	
	class Logger
	{
		public:
			Logger();
			~Logger();
		
			void AddBackend(LoggerBackend* newLog);
			void Log(int lvl, const char* format, ...);
			void Log(int lvl, int line, const char* func, const char* file, const char* format, ...);
			void Flush();
		protected:
			std::vector<LoggerBackend*> logs;
			std::vector<LoggerBackend*>::iterator it;
			std::vector<log_message> buffers;
	};
	extern Logger sore_log;
	extern FileLogger sore_file_log;
}
