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
	Server::Server(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm) : Task(gk), sm(_sm), broadcaster(gk, ENetAddress(), NULL), nextId(1), current(NULL), factory(NULL)
	{
		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = (unsigned int)sm.GetVariable("network", "server_port");
		server = enet_host_create(&address, 32, 0, 0);
		if(server == NULL)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "An error occured while creating server");
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
	
	void Server::UpdatePlayer(player_ref toUpdate)
	{
		SendBuffer playerUpdate;
		playerUpdate.AddUByte(DATATYPE_UPDATEPLAYER);
		playerUpdate.AddUByte(toUpdate->first);
		playerUpdate.AddUByte(toUpdate->second.playerState);
		playerUpdate.AddUByte(toUpdate->second.team);
		playerUpdate.AddUByte4(toUpdate->second.player_ip);
		playerUpdate.AddString1(toUpdate->second.name);
		for(player_ref i=playerList.begin();i!=playerList.end();i++)
		{
			if(i->first==toUpdate->first) continue;
			playerUpdate.Send(i->second.peer, 0, ENET_PACKET_FLAG_RELIABLE);
		}
	}
	
	void Server::SetGamestate(Gamestate* g)
	{
		current = g;
	}
	
	void SORE_Network::Server::SetFactory(GamestateFactory * gf)
	{
		factory = gf;
	}
	
	void Server::Frame(int elapsed)
	{
		ENetEvent event;
		
		if(server == NULL)
		{
			return;
		}
		while (enet_host_service (server, &event, 0) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					player newPlayer;
					newPlayer.player_ip = event.peer->address.host;
					enet_address_get_host_ip(&event.peer->address, newPlayer.player_ip_str, 16);
					ubyte newId;
					if(unusedIDs.size()>0)
					{
						newId = unusedIDs.front();
						unusedIDs.pop_front();
					}
					else
					{
						assert(nextId!=255);
						newId = nextId;
						nextId++;
					}
					assert(playerList.find(newId)==playerList.end());
					std::pair<player_ref, bool> iter = playerList.insert(std::pair<ubyte, player>(newId, newPlayer) );
					player_ref pos = iter.first;
					pos->second.id_iter = pos;
					pos->second.peer = event.peer;
					ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("A new client connected from %s") % newPlayer.player_ip_str);
					// Store any relevant client information here.
					event.peer->data = static_cast<void*>(&pos->second.id_iter);
					SendBuffer send_id;
					send_id.AddUByte(DATATYPE_CHANGEID);
					send_id.AddUByte(newId);
					send_id.Send(event.peer, 0, ENET_PACKET_FLAG_RELIABLE);
					UpdatePlayer(pos);
					SendGamestate(pos);
					PrintPlayers(SORE_Logging::LVL_INFO, playerList);
					break;
				}
				case ENET_EVENT_TYPE_RECEIVE:
				{
					std::string peer, channel;
					
					player_ref pos = GetPlayerRef(event.peer);
					if(pos->second.playerState == STATE_DISCONNECTING)
						break;
					peer = pos->second.name;
					if(event.channelID)
						channel = event.channelID;
					else
						channel = "0";
					ReceiveBuffer msg(*event.packet);
					ubyte dataType = msg.GetUByte();
					switch(dataType)
					{
						case DATATYPE_GAMESTATE_TRANSFER:
						{
							HandleGamestateTransfer(msg, pos);
							break;
						}
						case DATATYPE_GAMESTATE_DELTA:
						{
							HandleGamestateDelta(msg, pos);
							break;
						}
						case DATATYPE_PLAYERCHAT:
						{
							HandlePlayerChat(msg, pos);
							break;
						}
						case DATATYPE_CHANGEHANDLE:
						{
							//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: change handle");
							pos->second.name = msg.GetString1();
							PrintPlayers(SORE_Logging::LVL_INFO, playerList);
							//send confirmation
							SendBuffer data;
							data.AddUByte(DATATYPE_CHANGEHANDLE);
							data.AddString1(pos->second.name);
							data.Send(event.peer, 0, ENET_PACKET_FLAG_RELIABLE);
							//update other players
							UpdatePlayer(pos);
							break;
						}
						case DATATYPE_JOINSERVER:
							//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: join server");
							if(pos->second.playerState==STATE_CONNECTING)
								pos->second.playerState = STATE_CONNECTED;
							PrintPlayers(SORE_Logging::LVL_INFO, playerList);
							break;
						case DATATYPE_QUITSERVER:
							//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: quit server");
							enet_peer_disconnect(event.peer, 0);
							pos->second.playerState  = STATE_DISCONNECTING;
							break;
						case DATATYPE_CHANGETEAM:
						{
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: change team");
							ubyte teamId = msg.GetUByte();
							pos->second.team = teamId;
							UpdatePlayer(pos);
							break;
						}
						case DATATYPE_STATUSOBSERVE:
							//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: status observe");
							pos->second.playerState = STATE_OBSERVER;
							UpdatePlayer(pos);
							PrintPlayers(SORE_Logging::LVL_INFO, playerList);
							break;
						case DATATYPE_STATUSPLAY:
							//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: status play");
							pos->second.playerState = STATE_PLAYER;
							UpdatePlayer(pos);
							PrintPlayers(SORE_Logging::LVL_INFO, playerList);
							break;
						case DATATYPE_UPDATEPLAYER:
						case DATATYPE_DELETEPLAYER:
							ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Received corrupt packet from ip %s. (error: invalid datatype %u)") % pos->second.player_ip_str % static_cast<unsigned int>(dataType));
							break;
						default:
							ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Received corrupt packet from ip %s. (error: unknown datatype %u)") % pos->second.player_ip_str % static_cast<unsigned int>(dataType));
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
					std::string peer = pos->second.name;
					unusedIDs.push_back(pos->first);
					ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("%s disconected") % peer);
					// Reset the peer's client information.
					event.peer->data = NULL;
					SendBuffer killPlayer;
					killPlayer.AddUByte(DATATYPE_DELETEPLAYER);
					killPlayer.AddUByte(pos->first);
					killPlayer.Broadcast(server, 0, ENET_PACKET_FLAG_RELIABLE);
					playerList.erase(pos);
					PrintPlayers(SORE_Logging::LVL_INFO, playerList);
					break;
				}
				case ENET_EVENT_TYPE_NONE:
					break;
			}
		}
		//if(game)
			//BroadcastGamestateDelta();
		//if(game->Updated())
		//	BroadcastGamestateDelta();
	}
	
	size_t Server::NumPlayers() const
	{
		return playerList.size();
	}
	
	void Server::SetBroadcastCallback(boost::function<ENetBuffer (Server*)> c)
	{
		broadcaster.SetBroadcastCallback(c);
	}
	
	void Server::PrepareGamestateUpdate(SendBuffer& send)
	{
		if(current==NULL) return;
		send.AddUByte(DATATYPE_GAMESTATE_TRANSFER);
		current->Serialize(send);
	}
	
	void Server::SendGamestate(player_ref p)
	{
		SendBuffer send;
		PrepareGamestateUpdate(send);
		send.Send(p->second.peer, 1, 0);
	}

	void Server::SendGamestateDelta(GameInput* newInput, player_ref p)
	{
		if(current==NULL) return;
		Gamestate* currentCopy = factory->CreateGamestate(current);
		currentCopy->Simulate(newInput);
		SendBuffer send;
		send.AddUByte(DATATYPE_GAMESTATE_DELTA);
		current->Delta(currentCopy, send);
		send.Send(p->second.peer, 1, 0);
		delete currentCopy;
	}

	void Server::BroadcastGamestate()
	{
		SendBuffer send;
		PrepareGamestateUpdate(send);
		send.Broadcast(server, 1, 0);
	}
	
	void SORE_Network::Server::BroadcastGamestateDelta(Gamestate* old, player_ref toExclude)
	{
		SendBuffer send;
		send.AddUByte(DATATYPE_GAMESTATE_DELTA);
		current->Delta(old, send);
		for(player_ref it=playerList.begin();it!=playerList.end();++it)
		{
			if(it == toExclude)
				continue;
			send.Send(it->second.peer, 1, 0);
		}
	}

	
	void Server::HandlePlayerChat(ReceiveBuffer& msg, player_ref& peer)
	{
		//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: player chat");
		ubyte mask = msg.GetUByte();
		switch(mask)
		{
			case CHATMASK_ALL:
			{
				std::string chat = msg.GetString2();
				SendBuffer sendChat;
				sendChat.AddUByte(DATATYPE_PLAYERCHAT);
				sendChat.AddUByte(CHATMASK_ALL);
				sendChat.AddUByte(peer->first);
				sendChat.AddString2(chat);
				sendChat.Broadcast(server, 0, ENET_PACKET_FLAG_RELIABLE);
				break;
			}
			case CHATMASK_TEAM:
			{
				ubyte team = msg.GetUByte();
				std::string chat = msg.GetString2();
				SendBuffer sendChat;
				sendChat.AddUByte(DATATYPE_PLAYERCHAT);
				sendChat.AddUByte(CHATMASK_TEAM);
				sendChat.AddUByte(peer->first);
				sendChat.AddString2(chat);
				for(player_ref i=playerList.begin();i!=playerList.end();i++)
				{
					if(i->second.team == team)
						sendChat.Send(i->second.peer, 0, ENET_PACKET_FLAG_RELIABLE);
				}
				break;
			}
			case CHATMASK_WHISPER:
			{
				ubyte recipient = msg.GetUByte();
				std::string chat = msg.GetString2();
				SendBuffer sendChat;
				sendChat.AddUByte(DATATYPE_PLAYERCHAT);
				sendChat.AddUByte(CHATMASK_WHISPER);
				sendChat.AddUByte(peer->first);
				sendChat.AddString2(chat);
				sendChat.Send(playerList[recipient].peer, 0, ENET_PACKET_FLAG_RELIABLE);
				break;
			}
			default:
				ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Received corrupt packet from ip %s. (error: invalid chatmask %u)") % peer->second.player_ip_str % static_cast<unsigned int>(mask));
		}
	}
	
	void Server::HandleGamestateDelta(ReceiveBuffer& msg, player_ref& peer)
	{
		HandleGamestateTransfer(msg, peer);
	}
	
	void Server::HandleGamestateTransfer(ReceiveBuffer& msg, player_ref& peer)
	{
		//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: gamestate delta");
		if(peer->second.playerState!=STATE_PLAYER)
		{
			ENGINE_LOG(SORE_Logging::LVL_WARNING, "Cannot change gamestate if not playing");
			return;
		}
		if(current==NULL || factory==NULL)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Attempting to use nonexistent gamestate or factory");
			return;
		}
		Gamestate* currentCopy = factory->CreateGamestate(current);
		GameInput* input = factory->CreateGameInput(msg);
		current->Simulate(input);
		BroadcastGamestateDelta(currentCopy, peer);
		currentCopy->Deserialize(msg);
		gamestate_diff difference = current->Difference(currentCopy);
		switch(difference)
		{
			case NO_DIFFERENCE:
				break;
			case DEVIATION_DIFFERENCE:
				ENGINE_LOG(SORE_Logging::LVL_DEBUG2, "Sent correction packet");
				SendGamestateDelta(input, peer);
				break;
			case SEVERE_DIFFERENCE:
				ENGINE_LOG(SORE_Logging::LVL_DEBUG2, "Sent replacement packet");
				SendGamestate(peer);
				break;
			default:
				ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Received unknown difference: %d") % difference);
		}
		//delete currentCopy;
	}
}
