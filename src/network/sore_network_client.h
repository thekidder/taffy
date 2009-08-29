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

#ifndef SORE_NETWORK_CLIENT_H
#define SORE_NETWORK_CLIENT_H

#include <boost/function.hpp>
#include <enet/enet.h>

#include "../sore_task.h"
#include "../sore_settings.h"
#include "sore_network_buffers.h"
#include "sore_network_game.h"

namespace SORE_Network
{
	const std::string netVersion = "SORE_Net 0.2.2";
	
	const ubyte DATATYPE_START              = 1;
	
	//messages that change the gamestate
	const ubyte DATATYPE_GAMESTATE_TRANSFER = DATATYPE_START;
	const ubyte DATATYPE_GAMESTATE_DELTA    = DATATYPE_START + 1;
	
	//messages that affect current player
	const ubyte DATATYPE_CHANGEHANDLE       = DATATYPE_START + 2;
	const ubyte DATATYPE_STATUSCONNECTED    = DATATYPE_START + 3;
	const ubyte DATATYPE_QUITSERVER         = DATATYPE_START + 4;
	const ubyte DATATYPE_STATUSOBSERVE      = DATATYPE_START + 5;
	const ubyte DATATYPE_STATUSPLAY         = DATATYPE_START + 6;
	const ubyte DATATYPE_CHANGETEAM         = DATATYPE_START + 7;
	const ubyte DATATYPE_CHANGEID           = DATATYPE_START + 8;
	
	//player updates
	const ubyte DATATYPE_PLAYERCHAT         = DATATYPE_START + 9;
	const ubyte DATATYPE_UPDATEPLAYER       = DATATYPE_START + 10;
	const ubyte DATATYPE_DELETEPLAYER       = DATATYPE_START + 11;
	
	//server msic 
	const ubyte DATATYPE_CHANGESEED         = DATATYPE_START + 12;
	const ubyte DATATYPE_NETWORKVERSION     = DATATYPE_START + 13;
	
	const ubyte COMPRESSED = 1;

	typedef std::map<ubyte4, std::pair<unsigned int, net_buffer> > server_list; //(host, (last seen, message) )

	class Client : public SORE_Kernel::Task
	{
		public:
			Client(SORE_Utility::SettingsManager& _sm);
			~Client();
			
			server_list GetLANServers() const;
			
			//these should all be instantiated before Client is used
			void        SetGameInput(GameInput* i);
			void        SetFactory(GameFactory* gf);
			
			void        SetGameCallback(boost::function<void (Game*)> f);
			void        SetDisconnectCallback(boost::function<void (std::string)> f);
			
			//Connection Functions
			void        Connect(server_list::iterator it);
			void        Connect(ENetAddress address);
			void        Connect(ubyte4 host, ubyte2 port);
			void        ConnectDefaultHost(); //connect to host specified in settings manager
			
			//call this after gamestate is changed
			void        SendUpdate();
			
			void        Frame(int elapsed);
			const char* GetName() const {return "SORE Networking client task";}
			
		protected:
			void UpdateServers(int elapsed); //find broadcasting servers on LAN
			void Initialize(); //called after connecting and establishing a seed
			void SendNetworkVersions();
			ENetHost* client;
			ENetPeer* server;
			SORE_Utility::SettingsManager& sm;
			ENetSocket listener;
			server_list LAN;
			ENetAddress address;
			player me;
			ubyte myID;
			player_list otherPlayers;
			Game* game, *last;
			GameInput* input;
			GameFactory* factory;
			ubyte4 seed;
			
			boost::function<void (Game*)> callback;
			boost::function<void (std::string)> disconnectCallback;
			
			double bytesPerSec;
	};
}

#endif
