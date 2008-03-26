/***************************************************************************
 *   Copyright (C) 2008 by Adam Kidder                                     *
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

#ifndef  __SORE_GAMESTATE__
#define  __SORE_GAMESTATE__

#include <vector>
#include <string>
#include <map>
#ifdef _WIN32
#include <windows.h>
#endif
#include <enet/enet.h>

typedef   signed char  sbyte;
typedef unsigned char  ubyte;
typedef   signed short sbyte2;
typedef unsigned short ubyte2;
typedef   signed int   sbyte4;
typedef unsigned int   ubyte4;

//single and double precision floats
typedef float  float1;
typedef double float2;

const ubyte STATE_CONNECTING    = 0;
const ubyte STATE_CONNECTED     = 1;
const ubyte STATE_DISCONNECTING = 2;
const ubyte STATE_OBSERVER      = 3;
const ubyte STATE_PLAYER        = 4;

const ubyte CHATMASK_ALL        = 0;
const ubyte CHATMASK_TEAM       = 1;
const ubyte CHATMASK_WHISPER    = 2;

namespace SORE_Network
{
	typedef std::vector<ubyte> net_buffer;
	
	struct player;
	
	typedef std::map<ubyte, player> player_list;
	typedef player_list::iterator player_ref;
	
	struct player
	{
		player() : name("unnamed"), playerState(STATE_CONNECTING), team(0), peer(NULL), player_ip(0) {player_ip_str[0]='\0';}
		std::string name;
		ubyte playerState;
		ubyte team; //implementation defined
		
		//only used on server
		player_ref id_iter; //retrieved from server
		ENetPeer* peer;
		
		//network info
		enet_uint32 player_ip;
		char player_ip_str[16];
	};
	
	class Gamestate
	{
		public:
			virtual ~Gamestate() {}
			
			virtual net_buffer Serialize() = 0;
			virtual net_buffer SerializeDelta() = 0;
			virtual void   Deserialize(net_buffer& b) = 0;
			virtual void   DeserializeDelta(net_buffer& b) = 0;
			
		protected:
	};
}

#endif /*__SORE_GAMESTATE__*/
