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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <boost/tokenizer.hpp>

#include "sore_util.h"
#include "sore_logger.h"
#include "sore_fileio.h"

using namespace std;

namespace SORE_Utility
{
    float getRandomMinMax( float fMin, float fMax )
    {
        float fRandNum = (float)rand () / RAND_MAX;
        return fMin + (fMax - fMin) * fRandNum;
    }

    double getRandomMinMax( double dMin, double dMax )
    {
        double dRandNum = (double)rand () / RAND_MAX;
        return dMin + (dMax - dMin) * dRandNum;
    }

    int getRandomMinMax(int min, int max)
    {
        double randNum = (double)rand() / RAND_MAX;
        return static_cast<int>((min + (max - min) * randNum)+0.5);
    }

    int GetFileExt(const char* filename, char* ext)
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

    SORE_Utility::settings_map ParseIniFile(SORE_FileIO::InFile& file)
    {
        std::map<std::string, std::map<std::string, std::string> > list;
        size_t len;
        if(!file.strm().good())
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       boost::format("Could not load INI file"));
            return list;
        }
        else
        {
            const static unsigned int max_len = 128;
            char dataStr[max_len];
            file.strm().getline(dataStr, max_len);
            len = file.strm().gcount();

            std::string currSection;

            while(len>0 || !file.strm().eof())
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
                    list[currSection].insert(
                        std::pair<std::string, std::string>(name, value));
                }
                else
                {
                    setting = TrimString(setting);
                    //this describes a section heading
                    if(setting[0]=='[' && setting.find(']')!=std::string::npos)
                    {
                        currSection = TrimString(
                            setting.substr(1, setting.find(']')-1));
                        list.insert(
                            std::make_pair(
                                currSection, std::map<std::string, std::string>() ));
                    }
                    //treat this as a valueless setting
                    else if(!TrimString(setting).empty())
                    {
                        name = TrimString(setting);
                        value = "";
                        list[currSection].insert(
                            std::pair<std::string, std::string>(name, value));
                    }
                }
                file.strm().getline(dataStr, max_len);
                len = file.strm().gcount();
            }
        }
        return list;
    }

    std::string TrimString(std::string toTrim)
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

    std::vector<std::string> ParseList(std::string list)
    {
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        std::vector<std::string> result;
        boost::char_separator<char> sep(",{}");
        tokenizer separator(list, sep);
        for(tokenizer::iterator it=separator.begin();it!=separator.end(); ++it)
        {
            result.push_back(TrimString(*it));
        }
        return result;
    }
}
