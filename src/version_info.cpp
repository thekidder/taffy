#include "app_log.h"
#include "cmake_version.h"

#include <boost/format.hpp>

std::string GetVersionString()
{
    return boost::str(boost::format("%s %d.%d.%d (%s)")
            % VERSION_DISPLAY_NAME % VERSION_MAJOR % VERSION_MINOR % VERSION_PATCH % VERSION_COMMENT);
}

unsigned int GetVersionMajor()
{
    return VERSION_MAJOR;
}

unsigned int GetVersionMinor()
{
    return VERSION_MINOR;
}

unsigned int GetVersionPatch()
{
    return VERSION_PATCH;
}

std::string  GetVersionName()
{
    return VERSION_NAME;
}

std::string GetVersionDisplayName()
{
	return VERSION_DISPLAY_NAME;
}

std::string  GetVersionComment()
{
    return VERSION_COMMENT;
}
