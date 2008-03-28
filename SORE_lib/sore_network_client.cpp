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
	Client::Client(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm) : Task(gk), sm(_sm), myID(0), game(NULL)
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
			
			SendBuffer data;
			data.AddUByte(DATATYPE_JOINSERVER);
			data.Send(server, 0, ENET_PACKET_FLAG_RELIABLE);
			std::string name = sm.GetVariable("multiplayer", "name");
			
			data.Clear();
			data.AddUByte(DATATYPE_CHANGEHANDLE);
			data.AddString1(name);
			data.Send(server, 0, ENET_PACKET_FLAG_RELIABLE);
			
			data.Clear();
			data.AddUByte(DATATYPE_STATUSPLAY);
			//data.AddUByte(CHATMASK_ALL);
			//data.AddString2("Hello, world!");
			data.Send(server, 0, ENET_PACKET_FLAG_RELIABLE);
			
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
	
	void Client::SetGamestate(Gamestate* g)
	{
		game = g;
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
			
			if(LAN.find(remote.host)==LAN.end())
			{
				ENGINE_LOG(SORE_Logging::LVL_DEBUG1, boost::format("Found new server at %s") % addr );
			}
			LAN[remote.host].first = 0;
			LAN[remote.host].second = buffer;
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
		
		if(game && game->Update())
			PushGamestate();
		
		while (enet_host_service (client, &event, 0) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("A new client connected from %u:%u") % event.peer->address.host % event.peer->address.port);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
				{
					std::string peer, channel;
										
					ReceiveBuffer msg(*event.packet);
					ubyte dataType = msg.GetUByte();
					switch(dataType)
					{
						case DATATYPE_GAMESTATE_TRANSFER:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: gamestate transfer");
							if(game==NULL)
							{
								ENGINE_LOG(SORE_Logging::LVL_ERROR, "Attempting to change nonexistent gamestate");
								break;
							}
							game->Deserialize(msg, 0);
							break;
						case DATATYPE_GAMESTATE_DELTA:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: gamestate delta transfer");
							if(game==NULL)
							{
								ENGINE_LOG(SORE_Logging::LVL_ERROR, "Attempting to change nonexistent gamestate");
								break;
							}
							game->DeserializeDelta(msg, 0);
							break;
						case DATATYPE_PLAYERCHAT:
						{
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: player chat");
							ubyte mask = msg.GetUByte();
							ubyte sender = msg.GetUByte();
							std::string chat = msg.GetString2();
							std::string maskStr;
							switch(mask)
							{
								case CHATMASK_ALL:
									maskStr = "(All    )";
									break;
								case CHATMASK_TEAM:
									maskStr = "(Team   )";
									break;
								case CHATMASK_WHISPER:
									maskStr = "(Whisper)";
									break;
								default:
									ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Received corrupt packet. (error: invalid chatmask %u)") % static_cast<unsigned int>(mask));
									maskStr = "(???????)";
							}
							std::string name;
							if(sender == myID) name = me.name;
							else if(otherPlayers.find(sender)==otherPlayers.end())
							{
								ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Received corrupt packet. (error: invalid chat sender %u)") % static_cast<unsigned int>(sender));
								name = "Unknown player";
							}
							else name = otherPlayers[sender].name;
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Chat: %s %s: %s") % maskStr % name % chat);
							break;
						}
						case DATATYPE_UPDATEPLAYER:
						{
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Received packet: player update"));
							ubyte id = msg.GetUByte();
							otherPlayers[id].playerState = msg.GetUByte();
							otherPlayers[id].team = msg.GetUByte();
							otherPlayers[id].player_ip = msg.GetUByte4();
							ENetAddress temp;
							temp.host = otherPlayers[id].player_ip;
							temp.port = sm.GetVariable("network", "port");
							enet_address_get_host_ip(&temp, otherPlayers[id].player_ip_str, 16);
							otherPlayers[id].name = msg.GetString1();
							PrintPlayers(SORE_Logging::LVL_DEBUG3, otherPlayers);
							break;
						}
						case DATATYPE_DELETEPLAYER:
						{
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Received packet: player delete"));
							ubyte id = msg.GetUByte();
							if(otherPlayers.find(id)==otherPlayers.end())
							{
								ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Received corrupt packet. (error: invalid player to delete %u)") % static_cast<unsigned int>(id));
								break;
							}
							otherPlayers.erase(id);
							PrintPlayers(SORE_Logging::LVL_DEBUG3, otherPlayers);
							break;
						}
						case DATATYPE_CHANGEHANDLE:
						{
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: change handle");
							me.name = msg.GetString1();
							std::string msg = PrintPlayer(std::pair<ubyte, player>(myID, me));
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Player:\n" + msg);
							break;
						}
						case DATATYPE_CHANGEID:
						{
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: change ID");
							myID = msg.GetUByte();
							std::string msg = PrintPlayer(std::pair<ubyte, player>(myID, me));
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Player:\n" + msg);
							break;
						}
						case DATATYPE_JOINSERVER:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: join server");
							break;
						case DATATYPE_QUITSERVER:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: quit server");
							break;
						case DATATYPE_CHANGETEAM:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: change team");
							break;
						case DATATYPE_STATUSOBSERVE:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: status observe");
							break;
						case DATATYPE_STATUSPLAY:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: status play");
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
	
	void Client::PushGamestate()
	{
		if(game==NULL) return;
		SendBuffer header;
		header.AddUByte(DATATYPE_GAMESTATE_DELTA);
		SendBuffer state = game->SerializeDelta();
		header+=state;
		header.Send(server, 1, 0);
	}
}
