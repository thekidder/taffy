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
 * Currently SORE_Network provides Server and Client classes, which are used for
 * UDP/LAN connections and management. This is not permanent and may change at
 * any time.
 */

bool operator<(ENetAddress a, ENetAddress b); 

namespace SORE_Network
{
	
	const ubyte DATATYPE_START              = 1;
	
	//messages that change the gamestate
	const ubyte DATATYPE_GAMESTATE_TRANSFER = DATATYPE_START;
	const ubyte DATATYPE_GAMESTATE_DELTA    = DATATYPE_START + 1;
	
	//messages that affect current player
	const ubyte DATATYPE_CHANGEHANDLE       = DATATYPE_START + 3;
	const ubyte DATATYPE_JOINSERVER         = DATATYPE_START + 4;
	const ubyte DATATYPE_QUITSERVER         = DATATYPE_START + 5;
	const ubyte DATATYPE_STATUSOBSERVE      = DATATYPE_START + 6;
	const ubyte DATATYPE_STATUSPLAY         = DATATYPE_START + 7;
	const ubyte DATATYPE_CHANGETEAM         = DATATYPE_START + 8;
	const ubyte DATATYPE_CHANGEID           = DATATYPE_START + 10;
	
	//misc
	const ubyte DATATYPE_PLAYERCHAT         = DATATYPE_START + 2;
	const ubyte DATATYPE_UPDATEPLAYER       = DATATYPE_START + 9;
	
	void InitNetwork();
	
	ENetPacket* GetENetPacket(net_buffer& buf, enet_uint32 flags);
	std::string PrintPlayer(player_ref player);
	std::string PrintPlayer(std::pair<ubyte, player> p);
	void PrintPlayers(unsigned int lvl, player_list playerList);
	
	class NetworkBuffer
	{
		public:
			NetworkBuffer(ENetPacket& packet);
			
			//primatives
			ubyte  GetUByte();
			sbyte  GetByte();
			ubyte2 GetUByte2();
			sbyte2 GetByte2();
			ubyte4 GetUByte4();
			sbyte4 GetByte4();
			 
			 //conversions needed
			std::string GetString(size_t len); //string with given length
			std::string GetString1(); //string with one-byte (unsigned) preceding length
			std::string GetString2(); //string with two-byte (unsigned) preceding length
			
			float1 GetFloat1();
			float2 GetFloat2();
			
			size_t Remaining() const;
		protected:
			ubyte* data;
			size_t length;
			size_t remaining;
	};
	
	/*class NetworkConnection : public SORE_Kernel::Task
	{
		public:
			NetworkConnection(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm);
			~NetworkConnection();
			
			void Frame(int elapsed);
			const char* GetName() const {return "SORE Networking connection task";}
		protected:
			ENetHost* host;
			SORE_Utility::SettingsManager sm;
	};*/
	
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
			
			void SetBroadcastCallback(boost::function<ENetBuffer (Server*)> c);
			
			size_t NumPlayers() const;
			
			void Frame(int elapsed);
			const char* GetName() const {return "SORE Networking server task";}
		protected:
			player_ref GetPlayerRef(ENetPeer* peer);
			ENetHost* server;
			SORE_Utility::SettingsManager sm;
			UDPBroadcaster broadcaster;
			player_list playerList;
			std::deque<ubyte> unusedIDs;
	};
	
	typedef std::map<unsigned int, std::pair<unsigned int, net_buffer> > server_list;
	
	class Client : public SORE_Kernel::Task
	{
		public:
			Client(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm);
			~Client();
			
			void Frame(int elapsed);
			server_list GetLANServers() const;
			
			const char* GetName() const {return "SORE Networking client task";}
		protected:
			void UpdateServers(int elapsed); //find broadcasting servers on LAN
			ENetHost* client;
			ENetPeer* server;
			SORE_Utility::SettingsManager sm;
			ENetSocket listener;
			server_list LAN;
			ENetAddress address;
			player me;
			ubyte myID;
			player_list otherPlayers;
	};
}

#endif /*__SORE_NETWORK_H__*/
