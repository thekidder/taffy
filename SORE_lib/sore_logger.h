
//#include "fileio.h"
#include <vector>
#include <map>
#include <cstdio>

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
	
	const int BUFFER_LEN = 2048;
	
	struct log_buffer
	{
		int level;
		char buffer[BUFFER_LEN];
	};
	
	void InitLogging();
	void AddLogLevel(int lvl, const char* name); //name should be 8 characters long
	
	class LoggerBackend
	{
		public:
			virtual ~LoggerBackend() {}
			virtual void Write(int lvl, const char* string) = 0;
			virtual void Flush() = 0;
	};
	
	class FileLogger : public LoggerBackend
	{
		public:
			FileLogger(int lvl, const char* filename);
			~FileLogger();
			void SetLevel(int lvl);
			void Write(int lvl, const char* string);
			void Flush();
		protected:
			char file[256];
			int level;
			FILE* filePtr;
	};
	
	class Logger
	{
		public:
			Logger();
			~Logger() {}
		
			void AddBackend(LoggerBackend* newLog);
			void Log(int lvl, const char* format, ...);
			void Flush();
		protected:
			std::vector<LoggerBackend*> logs;
			std::vector<LoggerBackend*>::iterator it;
			std::vector<log_buffer> buffers;
	};
	
	
}
