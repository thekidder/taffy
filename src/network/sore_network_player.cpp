/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// $Id$

#include <boost/lexical_cast.hpp>

#include "../sore_logger.h"
#include "sore_network_player.h"

namespace SORE_Network
{
	std::string PrintPlayer(player_ref p)
	{
		std::pair<ubyte, player> i = *p;
		return PrintPlayer(i);
	}
	
	std::string PrintPlayer(std::pair<ubyte, player> p)
	{
		std::string msg;
		msg += p.second.player_ip_str;
		msg += "\t";
		msg += p.second.name;
		if(p.second.name.size()<8)
			msg += "\t";
		msg += "\t";
		msg += boost::lexical_cast<std::string>(static_cast<unsigned int>(p.second.playerState));
		msg += "\t";
		msg += boost::lexical_cast<std::string>(static_cast<unsigned int>(p.second.team));
		msg += "\t";
		msg += boost::lexical_cast<std::string>(static_cast<unsigned int>(p.first));
		msg += "\n";
		return msg;
	}
	
	void PrintPlayers(unsigned int lvl, player_list playerList)
	{
		std::string msg = "Printing players:\n";
		msg += "----------------------------------------------------\n";
		msg += "IP\t\tName\t\tState\tTeam\tID\n";
		for(player_ref i=playerList.begin();i!=playerList.end();i++)
		{
			msg += PrintPlayer(i);
		}
		msg += "----------------------------------------------------\n";
		ENGINE_LOG(lvl, msg);
	}

}
