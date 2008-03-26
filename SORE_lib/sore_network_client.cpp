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

#include "sore_network.h"

namespace SORE_Network
{
	Client::Client(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm) : Task(gk), sm(_sm), myID(0)
	{
		client = enet_host_create(NULL, 2, 0, 0);
		if(client == NULL)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "An error occuring while creating client");
		}
		ENetAddress address;
		ENetEvent event;
		std::string serverName = sm.GetVariable("network", "server");
		unsigned int port = static_cast<enet_uint16>(sm.GetVariable("network", "port"));
		enet_address_set_host (&address, serverName.c_str());
		address.port = port;
		
		server = enet_host_connect (client, &address, 2);
		if(server == NULL)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "No available peers for initiating a connection");
			return;
		}
		ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Waiting five seconds for connection to %s:%d...") % serverName % port);
		if (enet_host_service (client, &event, 5000) > 0 &&
					event.type == ENET_EVENT_TYPE_CONNECT)
		{
			ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Connection to %s:%d succeeded") % serverName % port);
			
			ENetPacket* packet;
			
			net_buffer data;
			data.push_back(DATATYPE_JOINSERVER);
			packet = GetENetPacket(data, ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send (server, 0, packet);
			
			std::string name = sm.GetVariable("multiplayer", "name");
			
			data.clear();
			data.push_back(DATATYPE_CHANGEHANDLE);
			data.push_back(static_cast<ubyte>(name.size()));
			for(size_t i=0;i<name.size();i++)
			{
				data.push_back(static_cast<ubyte>(name[i]));
			}
			//vdata = static_cast<void*>(&data[0]);
			//packet = enet_packet_create (vdata, data.size(), ENET_PACKET_FLAG_RELIABLE);
			packet = GetENetPacket(data, ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send (server, 0, packet);
			
			/*data.clear();
			data.push_back(DATATYPE_QUITSERVER);
			packet = GetENetPacket(data, ENET_PACK_FLAG_RELIABLE);
			enet_peer_send (server, 0, packet);*/
			
			enet_host_flush (client);
		}
		else
		{
			// Either the 5 seconds are up or a disconnect event was
			// received. Reset the peer in the event the 5 seconds
			// had run out without any significant event.
			enet_peer_reset (server);

			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Connection to %s:%d failed") % serverName % port);
		}
		
		address.host = ENET_HOST_ANY;
		address.port = static_cast<enet_uint16>(sm.GetVariable("network", "discover_port"));
		listener = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM, &address);
		enet_socket_set_option(listener, ENET_SOCKOPT_NONBLOCK, 1);
	}
	
	Client::~Client()
	{
		enet_socket_destroy(listener);
		ENetEvent event;
		enet_peer_disconnect(server, 0);
		
		// Allow up to 3 seconds for the disconnect to succeed
		//and drop any packets received packets.
		
		while (enet_host_service (client, &event, 3000) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					break;
				case ENET_EVENT_TYPE_RECEIVE:
				{
					enet_packet_destroy (event.packet);
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
					ENGINE_LOG(SORE_Logging::LVL_INFO, "Disconnection succeeded.");
					return;
				case ENET_EVENT_TYPE_NONE:
					break;
			}
		}
		// We've arrived here, so the disconnect attempt didn't
		// succeed yet.  Force the connection down
		enet_peer_reset (server);
		enet_host_destroy(client);
	}
	
	void Client::UpdateServers(int elapsed)
	{
		char addr[16];
		char data[256];
		ENetAddress remote;
		ENetBuffer buf;
		buf.data = data;
		buf.dataLength = 255;
		int remote_len = enet_socket_receive(listener, &remote, &buf, 1);
		if(remote_len>0)
		{
			net_buffer buffer;
			buffer.resize(remote_len);
			for(int i=0;i<remote_len;i++)
			{
				buffer[i] = data[i];
			}
			enet_address_get_host_ip(&remote, addr,15);
			//int len = strlen(buffer);
			//int realLen = remote_len;
			//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("receiving from %s: \"%s\" (length: %d, displayed length: %d)") % addr % buffer % realLen % len);
			
			if(LAN.find(remote.host)==LAN.end())
			{
				ENGINE_LOG(SORE_Logging::LVL_DEBUG1, boost::format("Found new server at %s") % addr );
			}
			LAN[remote.host].first = 0;
			LAN[remote.host].second = buffer;
			//LAN[remote.host].second.size() = buf.dataLength;
		}
		else if(remote_len==-1)
		{
			ENGINE_LOG(SORE_Logging::LVL_WARNING, "Socket receive failure");
		}
		server_list::iterator temp;
		for(server_list::iterator it=LAN.begin();it!=LAN.end();)
		{
			temp = it;
			it++;
			temp->second.first+= elapsed/10;
			if(temp->second.first>3000) //three second timeout before server disappears
			{
				ENetAddress tempAddr;
				tempAddr.host = temp->first;
				tempAddr.port = address.port;
				enet_address_get_host_ip(&tempAddr, addr,15);
				ENGINE_LOG(SORE_Logging::LVL_DEBUG1, boost::format("Lost server at %s:%d") % addr % address.port );
				LAN.erase(temp);
			}
		}
	}
	
	void Client::Frame(int elapsed)
	{
		UpdateServers(elapsed);
		
		ENetEvent event;
		
		while (enet_host_service (client, &event, 0) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("A new client connected from %u:%u") % event.peer->address.host % event.peer->address.port);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
				{
					std::string peer, channel;
										
					//player_ref pos = GetPlayerRef(event.peer);
					//peer = pos->name;
					//if(event.channelID)
					//	channel = event.channelID;
					//else
					//	channel = "0";
					NetworkBuffer msg(*event.packet);
					ubyte dataType = msg.GetUByte();
					switch(dataType)
					{
						case DATATYPE_GAMESTATE_TRANSFER:
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Received packet: gamestate transfer");
							break;
						case DATATYPE_GAMESTATE_DELTA:
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Received packet: gamestate delta transfer");
							break;
						case DATATYPE_PLAYERCHAT:
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Received packet: player chat");
							break;
						case DATATYPE_UPDATEPLAYER:
						{
							ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Received packet: player update (%u bytes)") % static_cast<unsigned int>(msg.Remaining()));
							ubyte id = msg.GetUByte();
							otherPlayers[id].playerState = msg.GetUByte();
							otherPlayers[id].team = msg.GetUByte();
							otherPlayers[id].player_ip = msg.GetUByte4();
							ENetAddress temp;
							temp.host = otherPlayers[id].player_ip;
							enet_address_get_host_ip(&temp, otherPlayers[id].player_ip_str, 15);
							otherPlayers[id].player_ip_str[16] = '\0';
							otherPlayers[id].name = msg.GetString1();
							break;
						}
						case DATATYPE_CHANGEHANDLE:
						{
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Received packet: change handle");
							me.name = msg.GetString1();
							std::string msg = PrintPlayer(std::pair<ubyte, player>(myID, me));
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Player:\n" + msg);
							break;
						}
						case DATATYPE_CHANGEID:
						{
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Received packet: change ID");
							myID = msg.GetUByte();
							std::string msg = PrintPlayer(std::pair<ubyte, player>(myID, me));
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Player:\n" + msg);
							break;
						}
						case DATATYPE_JOINSERVER:
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Received packet: join server");
							break;
						case DATATYPE_QUITSERVER:
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Received packet: quit server");
							break;
						case DATATYPE_CHANGETEAM:
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Received packet: change team");
							break;
						case DATATYPE_STATUSOBSERVE:
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Received packet: status observe");
							break;
						case DATATYPE_STATUSPLAY:
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Received packet: status play");
							break;
						default:
							ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Received corrupt packet. (error: unknown datatype %u)") % static_cast<unsigned int>(dataType));
							break;
					}
					if(msg.Remaining())
						ENGINE_LOG(SORE_Logging::LVL_WARNING, "Garbage remaining at end of packet");
					enet_packet_destroy (event.packet);
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
				{
					ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("disconected from server"));
					break;
				}
				case ENET_EVENT_TYPE_NONE:
					break;
			}
		}
	}
	
	server_list Client::GetLANServers() const
	{
		return LAN;
	}
}
