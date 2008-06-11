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

#ifndef  __SORE_NETWORK_H__
#define  __SORE_NETWORK_H__

#include "sore_task.h"
#include "sore_settings.h"
#include "sore_defines.h"
#include "sore_gamestate.h"
#include <enet/enet.h>
#include <deque>

/*
 * This subsystem is experimental. It provides a testing ground for networking
 * additions to the SORE library.
 * Current status: UDP broadcasting/listening works for discovering servers on LAN
 * Clients can connect/disconnect to servers, and change their handles
 * Code for packing/unpacking data into/from a network stream is present and should work
 * 
 */

//bool operator<(ENetAddress a, ENetAddress b); 

namespace SORE_Network
{
	const ubyte DATATYPE_START              = 1;
	
	//messages that change the gamestate
	const ubyte DATATYPE_GAMESTATE_TRANSFER = DATATYPE_START;
	const ubyte DATATYPE_GAMESTATE_DELTA    = DATATYPE_START + 1;
	
	//messages that affect current player
	const ubyte DATATYPE_CHANGEHANDLE       = DATATYPE_START + 2;
	const ubyte DATATYPE_JOINSERVER         = DATATYPE_START + 3;
	const ubyte DATATYPE_QUITSERVER         = DATATYPE_START + 4;
	const ubyte DATATYPE_STATUSOBSERVE      = DATATYPE_START + 5;
	const ubyte DATATYPE_STATUSPLAY         = DATATYPE_START + 6;
	const ubyte DATATYPE_CHANGETEAM         = DATATYPE_START + 7;
	const ubyte DATATYPE_CHANGEID           = DATATYPE_START + 8;
	
	//misc
	const ubyte DATATYPE_PLAYERCHAT         = DATATYPE_START + 9;
	const ubyte DATATYPE_UPDATEPLAYER       = DATATYPE_START + 10;
	const ubyte DATATYPE_DELETEPLAYER       = DATATYPE_START + 11;
	
	const ubyte DATATYPE_CHANGESEED         = DATATYPE_START + 12;
	
	void InitNetwork();
	
	std::string PrintPlayer(player_ref player);
	std::string PrintPlayer(std::pair<ubyte, player> p);
	void PrintPlayers(unsigned int lvl, player_list playerList);
	
	class Server;
	
	class UDPBroadcaster : public SORE_Kernel::Task
	{
		public:
			UDPBroadcaster(SORE_Kernel::GameKernel* gk, ENetAddress broadcastAddress, boost::function<ENetBuffer (Server*)> c);
			~UDPBroadcaster();
			
			void SetBroadcastAddress(ENetAddress broadcastAddress);
			void SetBroadcastCallback(boost::function<ENetBuffer (Server*)> c);
			void SetServer(Server* s);
			
			void Frame(int elapsed);
			const char* GetName() const {return "UDP Broadcaster";}
		protected:
			ENetSocket broadcaster;
			ENetAddress address;
			SORE_Kernel::task_ref thisTask;
			boost::function<ENetBuffer (Server*)> callback;
			Server* serv;
	};
	
	class Server : public SORE_Kernel::Task
	{
		public:
			Server(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm);
			~Server();
			
			void        SeedRNG(unsigned int s);
			
			void        SetBroadcastCallback(boost::function<ENetBuffer (Server*)> c);
			void        SetGamestate(Gamestate* g);
			void        SetFactory(GamestateFactory* gf);
			size_t      NumPlayers() const;
			
			//call this after gamestate is changed
			void        PushGamestate(); //send out new gamestate: will determine to send full/deltas to each client 
			
			void        Frame(int elapsed);
			const char* GetName() const {return "SORE Networking server task";}
		protected:
			//player methods
			player_ref GetPlayerRef(ENetPeer* peer);
			void       UpdatePlayer(player_ref toUpdate);
			
			//gamestate update methods
			void       PrepareGamestateUpdate(SendBuffer& send);
			void       SendGamestate(player_ref p);
			void       SendGamestateDelta(GameInput* newInput, player_ref p);
			void       BroadcastGamestateDelta(Gamestate* old, player_ref toExclude);
			void       BroadcastGamestate();
			
			//functions responsible for handling network messages
			void       HandlePlayerChat(ReceiveBuffer& msg, player_ref& peer);
			void       HandleGamestateTransfer(ReceiveBuffer& msg, player_ref& peer);
			void       HandleGamestateDelta(ReceiveBuffer& msg, player_ref& peer);
			
			void       SetGamestateFactory(GamestateFactory* gf);
			
			ENetHost* server;
			SORE_Utility::SettingsManager sm;
			UDPBroadcaster broadcaster;
			player_list playerList;
			std::deque<ubyte> unusedIDs;
			ubyte nextId;
			Gamestate* current;//, * last;
			GamestateFactory* factory;
			unsigned int lastTicks;
			ubyte4 seed;
	};
	
	typedef std::map<ubyte4, std::pair<unsigned int, net_buffer> > server_list; //(host, (last seen, message) )
	
	class Client : public SORE_Kernel::Task
	{
		public:
			Client(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm);
			~Client();
			
			server_list GetLANServers() const;
			
			//these should all be instantiated before Client is used
			void        SetGameInput(GameInput* i);
			void        SetFactory(GamestateFactory* gf);
			
			void        SetGamestateCallback(boost::function<void (Gamestate*)> f);
			
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
			ENetHost* client;
			ENetPeer* server;
			SORE_Utility::SettingsManager& sm;
			ENetSocket listener;
			server_list LAN;
			ENetAddress address;
			player me;
			ubyte myID;
			player_list otherPlayers;
			Gamestate* game;
			GameInput* input;
			GamestateFactory* factory;
			ubyte4 seed;
			boost::function<void (Gamestate*)> callback;
	};
}

#endif /*__SORE_NETWORK_H__*/
