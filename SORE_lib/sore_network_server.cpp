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
	Server::Server(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm) : Task(gk), sm(_sm), broadcaster(gk, ENetAddress(), NULL)
	{
		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = (unsigned int)sm.GetVariable("network", "server_port");
		server = enet_host_create(&address, 32, 0, 0);
		if(server == NULL)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "An error occuring while creating server");
		}
		else
		{
			ENGINE_LOG(SORE_Logging::LVL_INFO, "Created server");
		}
		
		ENetAddress a;
		a.port = (int)sm.GetVariable("network", "broadcast_port");
		std::string broadcastAddress = sm.GetVariable("network", "broadcast");
		enet_address_set_host(&a, broadcastAddress.c_str());
		broadcaster.SetBroadcastAddress(a);
		broadcaster.SetServer(this);
	}
	
	Server::~Server()
	{
		enet_host_destroy(server);
	}
	
	player_ref Server::GetPlayerRef(ENetPeer* peer)
	{
		player_ref* posptr = static_cast<player_ref*>(peer->data);
		player_ref pos = *posptr;
		return pos;
	}
	
	void Server::Frame(int elapsed)
	{
		ENetEvent event;
		
		while (enet_host_service (server, &event, 0) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					player newPlayer;
					newPlayer.player_ip = event.peer->address.host;
					enet_address_get_host_ip(&event.peer->address, newPlayer.player_ip_str, 15);
					newPlayer.player_ip_str[16] = '\0';
					ubyte newId;
					if(unusedIDs.size()>0)
					{
						newId = unusedIDs.front();
						unusedIDs.pop_front();
					}
					else
					{
						assert(playerList.size()<255);
						newId = static_cast<ubyte>(playerList.size()+1);
					}
					newPlayer.id = newId;
					player_ref pos = playerList.insert(playerList.end(), newPlayer);
					pos->id_iter = pos;
					ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("A new client connected from %s") % newPlayer.player_ip_str);
					// Store any relevant client information here.
					event.peer->data = static_cast<void*>(&pos->id_iter);
					net_buffer send_id;
					send_id.push_back(DATATYPE_CHANGEID);
					send_id.push_back(newId);
					ENetPacket* packet = GetENetPacket(send_id, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(event.peer, 0, packet);
					for(player_ref i=playerList.begin();i!=playerList.end();i++)
					{
						if(i==pos) continue;
						net_buffer playerUpdate;
						playerUpdate.push_back(DATATYPE_UPDATEPLAYER);
						playerUpdate.push_back(i->id);
						playerUpdate.push_back(i->playerState);
						playerUpdate.push_back(i->team);
						//playerUpdate.push_back(i->player_ip);
						playerUpdate.push_back(static_cast<ubyte>(i->name.size()));
						for(size_t j=0;j<i->name.size();j++)
						{
							playerUpdate.push_back(static_cast<ubyte>(i->name[j]));
						}
						ENetPacket* packet = GetENetPacket(playerUpdate, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(event.peer, 0, packet);
					}
					PrintPlayers(SORE_Logging::LVL_INFO, playerList);
					break;
				}
				case ENET_EVENT_TYPE_RECEIVE:
				{
					std::string peer, channel;
					
					player_ref pos = GetPlayerRef(event.peer);
					if(pos->playerState == STATE_DISCONNECTING)
						break;
					peer = pos->name;
					if(event.channelID)
						channel = event.channelID;
					else
						channel = "0";
					NetworkBuffer msg(*event.packet);
					ubyte dataType = msg.GetUByte();
					switch(dataType)
					{
						case DATATYPE_GAMESTATE_TRANSFER:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: gamestate transfer");
							if(pos->playerState!=STATE_PLAYER)
							{
								ENGINE_LOG(SORE_Logging::LVL_WARNING, "Cannot change gamestate if not playing");
								break;
							}
							break;
						case DATATYPE_GAMESTATE_DELTA:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: gamestate delta transfer");
							if(pos->playerState!=STATE_PLAYER)
							{
								ENGINE_LOG(SORE_Logging::LVL_WARNING, "Cannot change gamestate if not playing");
								break;
							}
							break;
						case DATATYPE_PLAYERCHAT:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: player chat");
							break;
						case DATATYPE_CHANGEHANDLE:
						{
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: change handle");
							pos->name = msg.GetString1();
							PrintPlayers(SORE_Logging::LVL_INFO, playerList);
							//send confirmation
							net_buffer data;
							data.push_back(DATATYPE_CHANGEHANDLE);
							data.push_back(static_cast<ubyte>(pos->name.size()));
							for(size_t i=0;i<pos->name.size();i++)
							{
								data.push_back(static_cast<ubyte>(pos->name[i]));
							}
							ENetPacket* packet = GetENetPacket(data, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(event.peer, 0, packet);
							break;
						}
						case DATATYPE_JOINSERVER:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: join server");
							if(pos->playerState==STATE_CONNECTING)
								pos->playerState = STATE_CONNECTED;
							PrintPlayers(SORE_Logging::LVL_INFO, playerList);
							break;
						case DATATYPE_QUITSERVER:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: quit server");
							enet_peer_disconnect(event.peer, 0);
							pos->playerState  = STATE_DISCONNECTING;
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
							ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Received corrupt packet from ip %s. (error: unknown datatype %u)") % pos->player_ip_str % static_cast<unsigned int>(dataType));
					}
					//ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("A packet of length %u containing %s was received from %s on channel %u") % event.packet->dataLength % (char*)event.packet->data % peer % channel);
					// Clean up the packet now that we're done using it.
					if(msg.Remaining())
						ENGINE_LOG(SORE_Logging::LVL_WARNING, "Garbage remaining at end of packet");
					enet_packet_destroy (event.packet);
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
				{
					player_ref pos = GetPlayerRef(event.peer);
					std::string peer = pos->name;
					ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("%s disconected") % peer);
					// Reset the peer's client information.
					event.peer->data = NULL;
					playerList.erase(pos);
					PrintPlayers(SORE_Logging::LVL_INFO, playerList);
					break;
				}
				case ENET_EVENT_TYPE_NONE:
					break;
			}
		}
	}
	
	size_t Server::NumPlayers() const
	{
		return playerList.size();
	}
	
	void Server::SetBroadcastCallback(boost::function<ENetBuffer (Server*)> c)
	{
		broadcaster.SetBroadcastCallback(c);
	}
}
