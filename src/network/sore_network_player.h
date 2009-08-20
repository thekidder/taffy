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

#ifndef SORE_NETWORK_PLAYER_H
#define SORE_NETWORK_PLAYER_H

#include <map>
#include <string>

#include <enet/enet.h>

#include "sore_network_common.h"

namespace SORE_Network
{
	struct player;
	
	typedef std::map<ubyte, player> player_list;
	typedef player_list::iterator player_ref;
	
	struct player
	{
		player() : name("unnamed"), playerState(STATE_CONNECTING), team(0), player_ip(0), peer(NULL), ticksToKick(0) 
		{player_ip_str[0]='\0';}
		std::string name;
		ubyte playerState;
		ubyte team; //implementation defined
		
		//network info
		enet_uint32 player_ip;
		char player_ip_str[16];
		
		//only used on server
		player_ref id_iter; //retrieved from server
		ENetPeer* peer;
		unsigned int ticksToKick;
	};

	std::string PrintPlayer(player_ref player);
	std::string PrintPlayer(std::pair<ubyte, player> p);
	void PrintPlayers(unsigned int lvl, player_list playerList);
}

#endif
