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

#include "sore_svnrev.h"
#include "sore_logger.h"
#include "sore_logginginit.h"

const unsigned int SORE_MAJOR = 0;
const unsigned int SORE_MINOR = 3;

const char*        SORE_BRANCH = "trunk"; //ex tag 1.2.1, branch 1.2, trunk, etc

namespace SORE_Logging
{
	void LoggingStartMessages()
	{
		ENGINE_LOG(SORE_Logging::LVL_INFO, 
							 boost::format("SORE Engine Version %d.%d (SVN revision %s, date %s, %s)")
							 % SORE_MAJOR % SORE_MINOR % SVN_REVSTR % SVN_REVDATE % SORE_BRANCH);
		if(SVN_MODIFIED)
		{
			ENGINE_LOG(SORE_Logging::LVL_WARNING, "One or more source files have been modified"
								 " since last commit (version number will be inaccurate)");
		}
	}
}
