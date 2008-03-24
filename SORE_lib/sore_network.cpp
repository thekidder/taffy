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
	
	UDPBroadcaster::UDPBroadcaster(SORE_Kernel::GameKernel* gk, ENetAddress broadcastAddress, boost::function<ENetBuffer (Server*)> c) : Task(gk), serv(NULL)
	{
		SetBroadcastAddress(broadcastAddress);
		SetBroadcastCallback(c);
		thisTask = gk->AddConstTask(11, 5000, this);
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
		int sent = enet_socket_send(broadcaster, &address, &data, 1);
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
	
	Server::Server(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm) : Task(gk), sm(_sm), broadcaster(gk, ENetAddress(), NULL), numPlayers(0)
	{
		/*ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = (unsigned int)sm.GetVariable("network", "port");
		server = enet_host_create(&address, 32, 0, 0);
		if(server == NULL)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "An error occuring while creating server");
		}
		else
		{
			ENGINE_LOG(SORE_Logging::LVL_INFO, "Created server");
		}*/
		ENetAddress a;
		a.port = (int)sm.GetVariable("network", "port");
		std::string broadcastAddress = sm.GetVariable("network", "broadcast");
		enet_address_set_host(&a, broadcastAddress.c_str());
		broadcaster.SetBroadcastAddress(a);
		broadcaster.SetServer(this);
	}
	
	Server::~Server()
	{
		//enet_host_destroy(server);
		
	}
	
	void Server::Frame(int elapsed)
	{
		/*static char peerName[256];
		
		ENetEvent event;
		
		while (enet_host_service (server, &event, 0) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					
					enet_address_get_host_ip(&event.peer->address, peerName, 255);
					ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("A new client connected from %s") % peerName);
					// Store any relevant client information here.
					event.peer->data = peerName;
					break;
				}
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
					std::string peer;
					if(event.peer->data!=NULL)
						peer = (char*)event.peer->data;
					else
						peer = "unknown client";
					ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("%s disconected") % peer);
					// Reset the peer's client information.
					event.peer->data = NULL;
			}
		}*/
	}
	
	unsigned int Server::NumPlayers() const
	{
		return numPlayers;
	}
	
	void Server::SetBroadcastCallback(boost::function<ENetBuffer (Server*)> c)
	{
		broadcaster.SetBroadcastCallback(c);
	}
	
	Client::Client(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm) : Task(gk), sm(_sm)
	{
		/*client = enet_host_create(NULL, 2, 0, 0);
		if(client == NULL)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "An error occuring while creating client");
		}
		ENetAddress address;
		ENetEvent event;
		std::string serverName = sm.GetVariable("network", "server");
		unsigned int port = (unsigned int)sm.GetVariable("network", "port");
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
			ENetPacket * packet = enet_packet_create ("packet", strlen ("packet") + 1, ENET_PACKET_FLAG_RELIABLE);

			// Send the packet to the peer over channel id 0
			// One could also broadcast the packet by
			// enet_host_broadcast (host, 0, packet);
			enet_peer_send (server, 0, packet);
			enet_host_flush (client);
		}
		else
		{
			// Either the 5 seconds are up or a disconnect event was
			// received. Reset the peer in the event the 5 seconds
			// had run out without any significant event.
			enet_peer_reset (server);

			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Connection to %s:%d failed") % serverName % port);
		}*/
		
		address.host = ENET_HOST_ANY;
		address.port = (int)sm.GetVariable("network", "port");
		listener = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM, &address);
		int result = enet_socket_set_option(listener, ENET_SOCKOPT_NONBLOCK, 1);
	}
	
	Client::~Client()
	{
		enet_socket_destroy(listener);
		/*ENetEvent event;
		enet_peer_disconnect(server, 0);
		
		// Allow up to 3 seconds for the disconnect to succeed
		//and drop any packets received packets.
		
		while (enet_host_service (client, &event, 3000) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_RECEIVE:
					enet_packet_destroy (event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					ENGINE_LOG(SORE_Logging::LVL_INFO, "Disconnection succeeded.");
					return;
			}
		}
		// We've arrived here, so the disconnect attempt didn't
		// succeed yet.  Force the connection down
		enet_peer_reset (server);
		enet_host_destroy(client);*/
	}
	
	void Client::Frame(int elapsed)
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
			char* buffer = new char[remote_len+1];
			strncpy(buffer, (char*)buf.data, remote_len);
			buffer[remote_len] = '\0';
			enet_address_get_host_ip(&remote, addr,15);
			int len = strlen(buffer);
			int realLen = remote_len;
			//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("receiving from %s: \"%s\" (length: %d, displayed length: %d)") % addr % buffer % realLen % len);
			
			unsigned int size = LAN.size();
			if(LAN.find(remote.host)==LAN.end())
			{
				ENGINE_LOG(SORE_Logging::LVL_DEBUG1, boost::format("Found new server at %s:%d") % addr % address.port );
			}
			LAN[remote.host].first = 0;
			//LAN[remote.host].second.data = static_cast<char*>(buf.data);
			LAN[remote.host].second.len = buf.dataLength;
			delete[] buffer;
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
		/*ENetEvent event;
		
		while (enet_host_service (client, &event, 0) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("A new client connected from %u:%u") % event.peer->address.host % event.peer->address.port);
					// Store any relevant client information here.
					event.peer->data = const_cast<void*>((const void*)"Client information");
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
					std::string peer;
					if(event.peer->data!=NULL)
						peer = (char*)event.peer->data;
					else
						peer = "unknown client";
					ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("%s disconected") % peer);
					// Reset the peer's client information.
					event.peer->data = NULL;
			}
		}*/
	}
	
	server_list Client::GetLANServers() const
	{
		return LAN;
	}
}
