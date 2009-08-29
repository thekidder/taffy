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

#include "sore_network_broadcaster.h"

namespace SORE_Network
{
	UDPBroadcaster::UDPBroadcaster(ENetAddress broadcastAddress, boost::function<ENetBuffer (Server*)> c) : serv(NULL)
	{
		SetBroadcastAddress(broadcastAddress);
		SetBroadcastCallback(c);
		broadcaster = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM, NULL);
		enet_socket_set_option(broadcaster, ENET_SOCKOPT_BROADCAST, 1);
	}
	
	UDPBroadcaster::~UDPBroadcaster()
	{
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
}
