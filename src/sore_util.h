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
// $Id$

#ifndef  SORE_UTIL_H
#define  SORE_UTIL_H

#include <cmath>
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace SORE_Utility
{
	typedef std::map<std::string, std::map<std::string, std::string> > settings_map;
	int GetFileExt(const char* filename, char* ext); //returns 0 on success

	double getRandomMinMax( double dMin, double fMax );
	float getRandomMinMax( float fMin, float fMax );
	int getRandomMinMax(int min, int max);

	settings_map ParseIniFile(const char* file);
	//trims a string of whitespace: ' ', '\r', and '\t'
	std::string TrimString(std::string toTrim);
	//breaks a string into a vector of strings delimited by , { and }. Each string is trimmed of whitespace.
	std::vector<std::string> ParseList(std::string list);
}

#endif
