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

bool operator<(ENetAddress a, ENetAddress b)
{
	if(a.host<b.host) return true;
	if(a.port<b.port) return true;
	return false;
}

namespace SORE_Network
{
	bool network_ok = false;
	
	void InitNetwork()
	{
		if (enet_initialize () != 0)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "An error occured while initializing SORE_Network");
			network_ok = false;
		}
		else
		{
			ENGINE_LOG(SORE_Logging::LVL_INFO, "Successfully initialized SORE_Network");
			network_ok = true;
			atexit (enet_deinitialize);
		}
	}
	
	ENetBuffer GetEnetBuffer(net_buffer buf)
	{
		ENetBuffer temp;
		ubyte* begin = &buf[0];
		temp.data = static_cast<void*>(begin);
		temp.dataLength = buf.size();
		return temp;
	}
	
	NetworkBuffer::NetworkBuffer(ENetPacket& packet)
	{
		data = static_cast<ubyte*>(packet.data);
		length = static_cast<size_t>(packet.dataLength);
		remaining = length;
	}
			
	ubyte NetworkBuffer::GetUByte()
	{
		assert(remaining>=1);
		remaining--;
		ubyte temp = *data;
		data++;
		return temp;
	}
	
	byte NetworkBuffer::GetByte()
	{
		assert(remaining>=1);
		remaining--;
		byte temp = static_cast<byte>(*data);
		data++;
		return temp;
	}
	
	ubyte2 NetworkBuffer::GetUByte2()
	{
		assert(remaining>=2);
		return 0;
	}
	
	byte2 NetworkBuffer::GetByte2()
	{
		assert(remaining>=2);
		return 0;
	}
	
	ubyte4 NetworkBuffer::GetUByte4()
	{
		assert(remaining>=4);
		return 0;
	}
	
	byte4 NetworkBuffer::GetByte4()
	{
		assert(remaining>=4);
		return 0;
	}
	
	std::string NetworkBuffer::GetString(size_t len)
	{
		std::string str = "";
		for(size_t i=0;i<len;i++)
		{
			str += static_cast<char>(GetByte());

		}
		return str;
	}
	
	std::string NetworkBuffer::GetString1()
	{
		size_t len = static_cast<size_t>(GetUByte());
		return GetString(len);
	}
	
	std::string NetworkBuffer::GetString2()
	{
		size_t len = static_cast<size_t>(GetUByte2());
		return GetString(len);
	}
			
	size_t NetworkBuffer::Remaining() const
	{
		return remaining;
	}
	
	UDPBroadcaster::UDPBroadcaster(SORE_Kernel::GameKernel* gk, ENetAddress broadcastAddress, boost::function<ENetBuffer (Server*)> c) : Task(gk), serv(NULL)
	{
		SetBroadcastAddress(broadcastAddress);
		SetBroadcastCallback(c);
		//thisTask = gk->AddConstTask(11, 5000, this);
		broadcaster = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM, NULL);
		enet_socket_set_option(broadcaster, ENET_SOCKOPT_BROADCAST, 1);
	}
	
	UDPBroadcaster::~UDPBroadcaster()
	{
		gk->RemoveTask(thisTask);
		enet_socket_destroy(broadcaster);
	}
	
	void UDPBroadcaster::Frame(int elapsed)
	{
		const char* invalidCallback = "invalid broadcast callback";
		ENetBuffer data;
		if(callback == NULL)
		{
			data.data = const_cast<void*>(static_cast<const void*>(invalidCallback));
			data.dataLength = strlen(invalidCallback)+1;
		}
		else
		{
			data = callback(serv);
			//data.data = const_cast<void*>(static_cast<const void*>(bdata));
			//data.dataLength = strlen(bdata) + 1;
		}
		enet_socket_send(broadcaster, &address, &data, 1);
	}
	
	void UDPBroadcaster::SetServer(Server* s)
	{
		serv = s;
	}
	
	void UDPBroadcaster::SetBroadcastAddress(ENetAddress broadcastAddress)
	{
		address = broadcastAddress;
	}
	
	void UDPBroadcaster::SetBroadcastCallback(boost::function<ENetBuffer (Server*)> c)
	{
		callback = c;
	}
	
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
	
	void Server::PrintPlayers(unsigned int lvl)
	{
		std::string msg = "Printing all players:\n";
		msg += "--------------------------------------------\n";
		msg += "IP\t\tName\t\tState\tTeam\n";
		for(player_ref i=playerList.begin();i!=playerList.end();i++)
		{
			msg += i->player_ip_str;
			msg += "\t";
			msg += i->name;
			if(i->name.size()<8)
				msg += "\t";
			msg += "\t";
			msg += boost::lexical_cast<std::string>(static_cast<unsigned int>(i->playerState));
			msg += "\t";
			msg += boost::lexical_cast<std::string>(static_cast<unsigned int>(i->team));
			msg += "\n";
		}
		msg += "--------------------------------------------\n";
		ENGINE_LOG(lvl, msg);
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
					player_ref pos = playerList.insert(playerList.end(), newPlayer);
					pos->id = pos;
					ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("A new client connected from %s") % newPlayer.player_ip_str);
					// Store any relevant client information here.
					event.peer->data = static_cast<void*>(&pos->id);
					PrintPlayers(SORE_Logging::LVL_INFO);
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
							PrintPlayers(SORE_Logging::LVL_INFO);
							break;
						}
						case DATATYPE_JOINSERVER:
							ENGINE_LOG(SORE_Logging::LVL_DEBUG3, "Received packet: join server");
							if(pos->playerState==STATE_CONNECTING)
								pos->playerState = STATE_CONNECTED;
							PrintPlayers(SORE_Logging::LVL_INFO);
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
					PrintPlayers(SORE_Logging::LVL_INFO);
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
	
	Client::Client(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm) : Task(gk), sm(_sm)
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
			
			net_buffer data;
			data.push_back(DATATYPE_JOINSERVER);
			void* vdata = static_cast<void*>(&data[0]);
			ENetPacket * packet = enet_packet_create (vdata, data.size(), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send (server, 0, packet);
			
			std::string name = sm.GetVariable("multiplayer", "name");
			
			data.clear();
			data.push_back(DATATYPE_CHANGEHANDLE);
			data.push_back(static_cast<ubyte>(name.size()));
			for(size_t i=0;i<name.size();i++)
			{
				data.push_back(static_cast<ubyte>(name[i]));
			}
			vdata = static_cast<void*>(&data[0]);
			packet = enet_packet_create (vdata, data.size(), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send (server, 0, packet);
			
			/*data.clear();
			data.push_back(DATATYPE_QUITSERVER);
			vdata = static_cast<void*>(&data[0]);
			packet = enet_packet_create (vdata, data.size(), ENET_PACKET_FLAG_RELIABLE);
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
					std::string peer, channel;
					size_t dataLeft = event.packet->dataLength;
					
					//player_ref pos = GetPlayerRef(event.peer);
					//peer = pos->name;
					//if(event.channelID)
					//	channel = event.channelID;
					//else
					//	channel = "0";
					ubyte* data = static_cast<ubyte*>(event.packet->data);
					ubyte dataType = data[0];
					dataLeft--;
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
						case DATATYPE_CHANGEHANDLE:
							ENGINE_LOG(SORE_Logging::LVL_INFO, "Received packet: change handle");
							break;
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
							//ENGINE_LOG(SORE_Logging::LVL_WARNING, boost::format("Received corrupt packet from ip %s. (error: unknown datatype %u)") % pos->player_ip_str % static_cast<unsigned int>(dataType));
							break;
					}
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
					if(event.peer->data!=NULL)
						peer = (char*)event.peer->data;
					else
						peer = "unknown client";
					if(event.channelID)
						channel = event.channelID;
					else
						channel = "0";
					ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("A packet of length %u containing %s was received from %s on channel %u") % event.packet->dataLength % (char*)event.packet->data % peer % channel);
					// Clean up the packet now that we're done using it.
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
