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

#ifndef SORE_NETWORK_SERVER_H
#define SORE_NETWORK_SERVER_H

#include <deque>

#include "../sore_task.h"
#include "../sore_settings.h"
#include "sore_network_buffers.h"
#include "sore_network_game.h"
#include "sore_network_player.h"

namespace SORE_Network
{
	class Server : public SORE_Kernel::Task
	{
		public:
			Server(SORE_Utility::SettingsManager& _sm);
			~Server();
			
			void        SeedRNG(unsigned int s);
			
			void        SetBroadcastCallback(boost::function<ENetBuffer (Server*)> c);
			void        SetGame(Game* g);
			void        SetFactory(GameFactory* gf);
			size_t      NumPlayers() const;
			
			//call this after gamestate is changed
			void        PushGame(); //send out new gamestate: will determine to send full/deltas to each client 
			
			void        Frame(int elapsed);
			const char* GetName() const {return "SORE Networking server task";}
		protected:
			//player methods
			player_ref GetPlayerRef(ENetPeer* peer);
			void       UpdatePlayer(player_ref toUpdate);
			void       UpdateDisconnects();
			
			bool       VersionMatch(player_ref pos, std::string type, std::string client, std::string server);
			void       Disconnect(player_ref player, std::string reason, unsigned int timeout); //timeout in ms
			
			//gamestate update methods
			void       PrepareGameUpdate(SendBuffer& send);
			void       SendGame(player_ref p);
			void       SendGameDelta(Game* old, player_ref p);
			void       BroadcastGameDelta(Game* old, player_ref toExclude);
			void       BroadcastGame();
			
			//functions responsible for handling network messages
			void       HandlePlayerChat(ReceiveBuffer& msg, player_ref& peer);
			void       HandleGameTransfer(ReceiveBuffer& msg, player_ref& peer);
			void       HandleGameDelta(ReceiveBuffer& msg, player_ref& peer);
			
			void       SetGameFactory(GameFactory* gf);
			
			ENetHost* server;
			SORE_Utility::SettingsManager sm;
			UDPBroadcaster broadcaster;
			player_list playerList;
			std::deque<ubyte> unusedIDs;
			ubyte nextId;
			Game* current;//, * last;
			std::map<ubyte, Game*> clientStates;
			GameFactory* factory;
			unsigned int lastTicks;
			ubyte4 seed;
	};
}

#endif
