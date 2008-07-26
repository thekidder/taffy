// $Id$

#include "sore_util.h"
#include "sore_logger.h"
#include "sore_fileio.h"

#include <boost/tokenizer.hpp>

float SORE_Utility::getRandomMinMax( float fMin, float fMax )
{
	float fRandNum = (float)rand () / RAND_MAX;
	return fMin + (fMax - fMin) * fRandNum;
}

double SORE_Utility::getRandomMinMax( double dMin, double dMax )
{
	double dRandNum = (double)rand () / RAND_MAX;
	return dMin + (dMax - dMin) * dRandNum;
}

int SORE_Utility::GetFileExt(const char* filename, char* ext)
{
	size_t len = strlen(filename);
	size_t i;
	for(i=len-1;i>=0;i--)
	{
		if(filename[i]=='.')
			break;
	}
	if(i==0) 
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, "No file extension");
		return -1;
	}
	if(len-i>10)
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, "Extension is too long");
		return -1;
	}
	strcpy(ext, filename+i+1);
	for(i=0;i<strlen(ext);i++)
	{
		ext[i] = tolower(ext[i]);
	}
	return 0;
}


SORE_Utility::settings_map SORE_Utility::ParseIniFile(const char* file)
{
	SORE_FileIO::file_ref settingsFile = SORE_FileIO::Open(file);
	std::map<std::string, std::map<std::string, std::string> > list;
	size_t len;
	if(settingsFile == 0)
	{
		ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Could not load INI file %s") % file);
		return list;
	}
	else
	{
		char dataStr[64];
		len = SORE_FileIO::Read(dataStr, 63, "\n", settingsFile);
		
		std::string currSection;
		
		while(len>0 || !SORE_FileIO::Eof(settingsFile))
		{
			std::string name, value, oldValue;
			std::string setting = dataStr;
			if(setting.find("#")!=std::string::npos)
				setting = setting.substr(0, setting.find("#"));
			size_t eqPos=setting.find('=');
			if(eqPos!=std::string::npos) //(name, value) pair
			{
				name=setting.substr(0,eqPos);
				value=setting.substr(eqPos+1);
				name = TrimString(name);
				value = TrimString(value);
				//ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Parsed setting: '%s:%s:%s'") % currSection % name % value);
				list[currSection].insert(std::pair<std::string, std::string>(name, value));
			}
			else
			{
				setting = TrimString(setting);
				if(setting[0]=='[' && setting.find(']')!=std::string::npos) //this describes a section heading
				{
					currSection = TrimString(setting.substr(1, setting.find(']')-1));
					list.insert(std::pair<std::string, std::map<std::string, std::string> >(currSection, std::map<std::string, std::string>() ));
					//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Setting section: '%s'") % currSection);
				}
				else if(!TrimString(setting).empty()) //treat this as a valueless setting
				{
					name = TrimString(setting);
					value = "";
					//ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Parsed setting: '%s:%s:%s'") % currSection % name % value);
					list[currSection].insert(std::pair<std::string, std::string>(name, value));
				}
			}
			len = SORE_FileIO::Read(dataStr, 63, "\n", settingsFile);
		}
		
		SORE_FileIO::Close(settingsFile);
	}
	return list;
}

std::string SORE_Utility::TrimString(std::string toTrim)
{
	std::string trimmed = toTrim;
	size_t pos;
	while((pos=trimmed.find(' '))!=trimmed.npos)
	{
		trimmed.erase(pos,1);
	}
	while((pos=trimmed.find('\t'))!=trimmed.npos)
	{
		trimmed.erase(pos,1);
	}
	while((pos=trimmed.find('\r'))!=trimmed.npos) //windows files
	{
		trimmed.erase(pos,1);
	}
	return trimmed;
}

std::vector<std::string> SORE_Utility::ParseList(std::string list)
{
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	std::vector<std::string> result;
	boost::char_separator<char> sep(",{}");
	tokenizer separator(list, sep);
	for(tokenizer::iterator it=separator.begin();it!=separator.end(); ++it)
	{
		result.push_back(*it);
	}
	return result;
}
