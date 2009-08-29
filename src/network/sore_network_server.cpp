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

#include <cstdio>

#include "../sore_logger.h"
#include "../sore_timing.h"
#include "sore_network_broadcaster.h"
#include "sore_network_client.h"
#include "sore_network_server.h"

namespace SORE_Network
{
	Server::Server(SORE_Utility::SettingsManager& _sm) : sm(_sm), broadcaster(ENetAddress(), NULL), nextId(1), current(NULL), factory(NULL), lastTicks(0), seed(0)
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
		for(std::map<ubyte, Game*>::iterator it=clientStates.begin();it!=clientStates.end();++it)
		{
			delete it->second;
		}
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
		SendBuffer header;
		header.AddUByte(COMPRESSED);
		playerUpdate.Compress(header);
		for(player_ref i=playerList.begin();i!=playerList.end();i++)
		{
			if(i->first==toUpdate->first) continue;
			header.Send(i->second.peer, 0, ENET_PACKET_FLAG_RELIABLE);
		}
	}
	
	void SORE_Network::Server::SeedRNG(unsigned int s)
	{
		seed = static_cast<ubyte4>(s);
		ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Seeding RNG with seed %u") % seed);
		srand(static_cast<unsigned int>(seed));
	}
	
	void Server::SetGame(Game* g)
	{
		current = g;
	}
	
	void SORE_Network::Server::SetFactory(GameFactory * gf)
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
		
		UpdateDisconnects();
		
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
					SendBuffer header;
					header.AddUByte(COMPRESSED);
					send_id.Compress(header);
					header.Send(event.peer, 0, ENET_PACKET_FLAG_RELIABLE);
					UpdatePlayer(pos);
					SendGame(pos);
					PrintPlayers(SORE_Logging::LVL_INFO, playerList);
					send_id.Clear();
					header.Clear();
					send_id.AddUByte(DATATYPE_CHANGESEED);
					send_id.AddUByte4(seed);
					header.AddUByte(COMPRESSED);
					send_id.Compress(header);
					header.Send(event.peer, 0, ENET_PACKET_FLAG_RELIABLE);
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
					ReceiveBuffer raw(*event.packet);
					ubyte dataHead = raw.GetUByte();
					ReceiveBuffer msg(raw, dataHead!=0);
					ubyte dataType = msg.GetUByte();
					switch(dataType)
					{
						case DATATYPE_NETWORKVERSION:
						{
							//client is sending network version...
							if(current==NULL)
							{
								ENGINE_LOG(SORE_Logging::LVL_ERROR, "Attempting to verify version of nonexistent gamestate");
								break;
							}
							std::string remoteVersion = msg.GetString1();
							std::string serverVersion = current->NetworkVersion();
							
							std::string soreVersion = msg.GetString1();
							if(!VersionMatch(pos, "game", remoteVersion, serverVersion))
							{
								std::string mismatch = "(server: " + serverVersion + ", client: " + remoteVersion + ")";
								Disconnect(pos, "Incorrect game version " + mismatch, 5000);
								break;
							}
							
							serverVersion = SORE_Network::netVersion;
							if(!VersionMatch(pos, "SORE_Net", soreVersion, serverVersion))
							{
								std::string mismatch = "(server: " + serverVersion + ", client: " + soreVersion + ")";
								Disconnect(pos, "Incorrect SORE_Net version " + mismatch, 5000);
								break;
							}
							
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Verified game/SORE_Net versions of player " + pos->second.name);
							
							ubyte oldState = pos->second.playerState;
							if(pos->second.playerState==STATE_CONNECTING)
							{
								pos->second.playerState = STATE_CONNECTED;
								current->OnPlayerStateChange(pos, oldState);
							}
							PrintPlayers(SORE_Logging::LVL_INFO, playerList);
							SendBuffer status;
							status.AddUByte(DATATYPE_STATUSCONNECTED);
							SendBuffer header;
							header.AddUByte(COMPRESSED);
							status.Compress(header);
							header.Send(pos->second.peer, 0, ENET_PACKET_FLAG_RELIABLE);
							UpdatePlayer(pos);
							
							break;
						}
						case DATATYPE_GAMESTATE_TRANSFER:
						{
							HandleGameTransfer(msg, pos);
							break;
						}
						case DATATYPE_GAMESTATE_DELTA:
						{
							HandleGameDelta(msg, pos);
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
							if(pos->second.playerState == STATE_CONNECTING)
							{
								ENGINE_LOG(SORE_Logging::LVL_INFO, "Cannot change handle until fully connected");
								break;
							}
							pos->second.name = msg.GetString1();
							PrintPlayers(SORE_Logging::LVL_INFO, playerList);
							//send confirmation
							SendBuffer data;
							data.AddUByte(DATATYPE_CHANGEHANDLE);
							data.AddString1(pos->second.name);
							SendBuffer header;
							header.AddUByte(COMPRESSED);
							data.Compress(header);
							header.Send(event.peer, 0, ENET_PACKET_FLAG_RELIABLE);
							//update other players
							UpdatePlayer(pos);
							break;
						}
						/*case DATATYPE_JOINSERVER: //deprecated - what's the point of this, really?
						{
							//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: join server");
							ubyte oldState = pos->second.playerState;
							if(pos->second.playerState==STATE_CONNECTING)
							{
								pos->second.playerState = STATE_CONNECTED;
								current->OnPlayerStateChange(pos, oldState);
							}
							PrintPlayers(SORE_Logging::LVL_INFO, playerList);
							break;
						}*/
						case DATATYPE_QUITSERVER:
						{
							//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: quit server");
							enet_peer_disconnect(event.peer, 0);
							ubyte oldState = pos->second.playerState;
							pos->second.playerState  = STATE_DISCONNECTING;
							current->OnPlayerStateChange(pos, oldState);
							break;
						}
						case DATATYPE_CHANGETEAM:
						{
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: change team");
							if(pos->second.playerState == STATE_CONNECTING)
							{
								ENGINE_LOG(SORE_Logging::LVL_INFO, "Cannot change handle until fully connected");
								break;
							}
							ubyte teamId = msg.GetUByte();
							pos->second.team = teamId;
							UpdatePlayer(pos);
							break;
						}
						case DATATYPE_STATUSOBSERVE:
						{
							//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: status observe");
							if(pos->second.playerState == STATE_CONNECTING)
							{
								ENGINE_LOG(SORE_Logging::LVL_INFO, "Cannot change status until fully connected");
								break;
							}
							ubyte oldState = pos->second.playerState;
							pos->second.playerState = STATE_OBSERVER;
							current->OnPlayerStateChange(pos, oldState);
							UpdatePlayer(pos);
							PrintPlayers(SORE_Logging::LVL_INFO, playerList);
							break;
						}
						case DATATYPE_STATUSPLAY:
						{
							//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: status play");
							if(pos->second.playerState == STATE_CONNECTING)
							{
								ENGINE_LOG(SORE_Logging::LVL_INFO, "Cannot change status until fully connected");
								break;
							}
							ubyte oldState = pos->second.playerState;
							pos->second.playerState = STATE_PLAYER;
							current->OnPlayerStateChange(pos, oldState);
							UpdatePlayer(pos);
							PrintPlayers(SORE_Logging::LVL_INFO, playerList);
							break;
						}
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
					if(pos->second.playerState != STATE_DISCONNECTING)
					{
						ubyte old = pos->second.playerState;
						pos->second.playerState = STATE_DISCONNECTING;
						current->OnPlayerStateChange(pos, old);
					}
					ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("%s disconected") % peer);
					// Reset the peer's client information.
					event.peer->data = NULL;
					SendBuffer killPlayer;
					killPlayer.AddUByte(DATATYPE_DELETEPLAYER);
					killPlayer.AddUByte(pos->first);
					SendBuffer header;
					header.AddUByte(COMPRESSED);
					killPlayer.Compress(header);
					header.Broadcast(server, 0, ENET_PACKET_FLAG_RELIABLE);
					playerList.erase(pos);
					PrintPlayers(SORE_Logging::LVL_INFO, playerList);
					break;
				}
				case ENET_EVENT_TYPE_NONE:
					break;
			}
		}
		current->SimulateTime(elapsed);
	}
	
	size_t Server::NumPlayers() const
	{
		return playerList.size();
	}
	
	void Server::SetBroadcastCallback(boost::function<ENetBuffer (Server*)> c)
	{
		broadcaster.SetBroadcastCallback(c);
	}
	
	bool Server::VersionMatch(player_ref pos, std::string type, std::string client, std::string server)
	{
		if(client != server)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Client %s has a mismatched %s version") % pos->second.name % type);
			return false;
		}
		else
		{
			ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Verified %s version of player %s (%s)") % type % pos->second.name % server);
			return true;
		}
	}
	
	void Server::UpdateDisconnects()
	{
		for(player_ref it=playerList.begin();it!=playerList.end();++it)
		{
			unsigned int ticks = SORE_Timing::GetGlobalTicks();
			if(it->second.playerState == STATE_DISCONNECTING && ticks > it->second.ticksToKick)
				enet_peer_reset (it->second.peer);
		}
	}
	
	void Server::Disconnect(player_ref player, std::string reason, unsigned int timeout)
	{
		ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("Disconnecting player %s for: \"%s\"") % player->second.name % reason);
		
		ubyte oldState = player->second.playerState;
		player->second.playerState = STATE_DISCONNECTING;
		current->OnPlayerStateChange(player, oldState);
		player->second.ticksToKick = SORE_Timing::GetGlobalTicks() + timeout*10;
		
		SendBuffer kickPacket;
		kickPacket.AddUByte(DATATYPE_QUITSERVER);
		kickPacket.AddString1(reason);
		SendBuffer header;
		header.AddUByte(COMPRESSED);
		kickPacket.Compress(header);
		header.Send(player->second.peer, 0, ENET_PACKET_FLAG_RELIABLE);
	}
	
	void Server::PrepareGameUpdate(SendBuffer& send)
	{
		if(current==NULL) return;
		send.AddUByte(DATATYPE_GAMESTATE_TRANSFER);
		current->Serialize(send);
	}
	
	void Server::SendGame(player_ref p)
	{
		//ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("sent replacement packet to %s") % p->second.name);
		SendBuffer send;
		PrepareGameUpdate(send);
		SendBuffer header;
		header.AddUByte(COMPRESSED);
		send.Compress(header);
		header.Send(p->second.peer, 1, 0);
	}

	void Server::SendGameDelta(Game* old, player_ref p)
	{
		if(current==NULL) return;
		//ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("sent correction packet to %s") % p->second.name);
		SendBuffer send;
		send.AddUByte(DATATYPE_GAMESTATE_DELTA);
		current->Delta(old, send);
		SendBuffer header;
		header.AddUByte(COMPRESSED);
		send.Compress(header);
		header.Send(p->second.peer, 1, 0);
	}

	void Server::BroadcastGame()
	{
		//ENGINE_LOG(SORE_Logging::LVL_DEBUG2, "broadcast replacement packet");
		SendBuffer send;
		PrepareGameUpdate(send);
		SendBuffer header;
		header.AddUByte(COMPRESSED);
		send.Compress(header);
		header.Broadcast(server, 1, 0);
	}
	
	void Server::BroadcastGameDelta(Game* old, player_ref toExclude)
	{
		SendBuffer send;
		send.AddUByte(DATATYPE_GAMESTATE_DELTA);
		current->Delta(old, send);
		for(player_ref it=playerList.begin();it!=playerList.end();++it)
		{
			if(it == toExclude)
				continue;
			//ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("broadcast correction packet to %s") % it->second.name);

			SendBuffer header;
			header.AddUByte(COMPRESSED);
			send.Compress(header);
			header.Send(it->second.peer, 1, 0);
		}
	}

	
	void Server::HandlePlayerChat(ReceiveBuffer& msg, player_ref& peer)
	{
		//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: player chat");
		if(peer->second.playerState == STATE_CONNECTING)
		{
			ENGINE_LOG(SORE_Logging::LVL_INFO, "Cannot chat until fully connected");
			return;
		}
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
				SendBuffer header;
				header.AddUByte(COMPRESSED);
				sendChat.Compress(header);
				header.Broadcast(server, 0, ENET_PACKET_FLAG_RELIABLE);
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
				SendBuffer header;
				header.AddUByte(COMPRESSED);
				sendChat.Compress(header);
				for(player_ref i=playerList.begin();i!=playerList.end();i++)
				{
					if(i->second.team == team)
						header.Send(i->second.peer, 0, ENET_PACKET_FLAG_RELIABLE);
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
				SendBuffer header;
				header.AddUByte(COMPRESSED);
				sendChat.Compress(header);
				header.Send(playerList[recipient].peer, 0, ENET_PACKET_FLAG_RELIABLE);
				break;
			}
			default:
				ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Received corrupt packet from ip %s. (error: invalid chatmask %u)") % peer->second.player_ip_str % static_cast<unsigned int>(mask));
		}
	}
	
	void Server::HandleGameDelta(ReceiveBuffer& msg, player_ref& peer)
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
		//Game* currentCopy = factory->CreateGame(current);
		if(clientStates.find(peer->first)==clientStates.end())
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "cannot find client state!");
			return;
		}
		Game* client = clientStates[peer->first];
		GameInput* input = factory->CreateGameInput(msg);
		current->SimulateInput(peer->first, input);
		//BroadcastGameDelta(currentCopy, peer);
		client->Deserialize(msg);
		gamestate_diff difference = current->Difference(client);
		if(current->ForceCompleteUpdate())
		{
			SendGame(peer);
			//BroadcastGame();
		}
		else
		{
			switch(difference)
			{
				case NO_DIFFERENCE:
					break;
				case DEVIATION_DIFFERENCE:
					SendGameDelta(client, peer);
					break;
				case SEVERE_DIFFERENCE:
					SendGame(peer);
					break;
				default:
					ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Received unknown difference: %d") % difference);
			}
		}
		//delete currentCopy;
		delete input;
	}
	
	void Server::HandleGameTransfer(ReceiveBuffer& msg, player_ref& peer)
	{
		//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: gamestate transfer");
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
		ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Now setting up client state...");
		Game* client = factory->CreateGame();
		client->Deserialize(msg);
		clientStates.insert(std::make_pair(peer->first, client));
	}
}
