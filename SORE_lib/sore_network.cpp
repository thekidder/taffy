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
	
	ENetPacket* GetENetPacket(net_buffer& buf, enet_uint32 flags)
	{
		ENetPacket* temp;
		ubyte* begin = &buf[0];
		void* vdata = static_cast<void*>(begin);
		temp = enet_packet_create(vdata, buf.size(), flags);
		return temp;
	}
	
	std::string PrintPlayer(player_ref p)
	{
		player* i = &(*p);
		return PrintPlayer(i);
	}
	
	std::string PrintPlayer(player* p)
	{
		std::string msg;
		msg += p->player_ip_str;
		msg += "\t";
		msg += p->name;
		if(p->name.size()<8)
			msg += "\t";
		msg += "\t";
		msg += boost::lexical_cast<std::string>(static_cast<unsigned int>(p->playerState));
		msg += "\t";
		msg += boost::lexical_cast<std::string>(static_cast<unsigned int>(p->team));
		msg += "\t";
		msg += boost::lexical_cast<std::string>(static_cast<unsigned int>(p->id));
		msg += "\n";
		return msg;
	}
	
	void PrintPlayers(unsigned int lvl, player_list playerList)
	{
		std::string msg = "Printing players:\n";
		msg += "----------------------------------------------------\n";
		msg += "IP\t\tName\t\tState\tTeam\tID\n";
		for(player_ref i=playerList.begin();i!=playerList.end();i++)
		{
			msg += PrintPlayer(i);
		}
		msg += "----------------------------------------------------\n";
		ENGINE_LOG(lvl, msg);
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
}
