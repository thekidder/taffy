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

#ifndef  SORE_UTIL_H
#define  SORE_UTIL_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <map>
#include <string>
#include <vector>

#include <sore_dll.h>
#include <sore_fileio.h>

namespace SORE_Utility
{
    typedef SORE_EXPORT std::map<std::string, std::map<std::string, std::string> >
    settings_map;

    //returns 0 on success
    int          SORE_EXPORT GetFileExt(const char* filename, char* ext);

    double       SORE_EXPORT getRandomMinMax( double dMin, double fMax );
    float        SORE_EXPORT getRandomMinMax( float fMin, float fMax );
    int          SORE_EXPORT getRandomMinMax(int min, int max);

    settings_map SORE_EXPORT ParseIniFile(SORE_FileIO::InFile& file);
    //trims a string of whitespace: ' ', '\r', and '\t'
    std::string  SORE_EXPORT TrimString(std::string toTrim);
    //breaks a string into a vector of strings delimited by , { and }.
    //Each string is trimmed of whitespace.
    std::vector<std::string> SORE_EXPORT ParseList(std::string list);
}

#endif
