// $Id$

#include "app_log.h"
#include "project_name_svnrev.h"

const unsigned int VERSION_MAJOR = 0;
const unsigned int VERSION_MINOR = 0;

const char*        VERSION_BRANCH = "trunk"; //ex tag 1.2.1, branch 1.2, trunk, etc

void PrintVersionInfo()
{
	APP_LOG(SORE_Logging::LVL_INFO, 
						 boost::format("Game Version %d.%d (SVN revision %s, date %s, %s)")
						 % VERSION_MAJOR % VERSION_MINOR % SVN_REVSTR % SVN_REVDATE % VERSION_BRANCH);
	if(SVN_MODIFIED)
	{
		APP_LOG(SORE_Logging::LVL_WARNING, "One or more source files have been modified"
							 " since last commit (version number will be inaccurate)");
	}
}
