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
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
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

#include <boost/tokenizer.hpp>

#include <sore_util.h>
#include <sore_logger.h>
#include <sore_fileio.h>

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
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not load INI file");
            return list;
        }
        else
        {
            const static unsigned int max_len = 128;
            char dataStr[max_len];
            file.strm().getline(dataStr, max_len);
            len = static_cast<size_t>(file.strm().gcount());

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
                    if(setting.length() && setting[0]=='[' && setting.find(']')!=std::string::npos)
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
                len = static_cast<size_t>(file.strm().gcount());
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
