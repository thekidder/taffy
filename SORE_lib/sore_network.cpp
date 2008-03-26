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
		std::pair<ubyte, player> i = *p;
		return PrintPlayer(i);
	}
	
	std::string PrintPlayer(std::pair<ubyte, player> p)
	{
		std::string msg;
		msg += p.second.player_ip_str;
		msg += "\t";
		msg += p.second.name;
		if(p.second.name.size()<8)
			msg += "\t";
		msg += "\t";
		msg += boost::lexical_cast<std::string>(static_cast<unsigned int>(p.second.playerState));
		msg += "\t";
		msg += boost::lexical_cast<std::string>(static_cast<unsigned int>(p.second.team));
		msg += "\t";
		msg += boost::lexical_cast<std::string>(static_cast<unsigned int>(p.first));
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
	
	SendBuffer::SendBuffer()
	{
	}
			
	void SendBuffer::AddUByte (ubyte b)
	{
		buf.push_back(b);
	}
	
	void SendBuffer::AddByte  (sbyte b)
	{
		buf.push_back(static_cast<ubyte>(b));
	}
			
	void SendBuffer::AddUByte2(ubyte2 b)
	{
		ubyte2 n = htons(b);
		size_t len = buf.size();
		buf.resize(len+2);
		ubyte2* ptr = reinterpret_cast<ubyte2*>(&buf[len]);
		*ptr = n;
	}

	void SendBuffer::AddByte2 (sbyte2 b)
	{
		AddUByte2(static_cast<ubyte2>(b));
	}

	void SendBuffer::AddUByte4(ubyte4 b)
	{
		ubyte4 n = htonl(b);
		size_t len = buf.size();
		buf.resize(len+4);
		ubyte4* ptr = reinterpret_cast<ubyte4*>(&buf[len]);
		*ptr = n;
	}

	void SendBuffer::AddByte4 (sbyte4 b)
	{
		AddUByte4(static_cast<ubyte4>(b));
	}

	void SendBuffer::AddString(std::string str)
	{
		assert(str.size()<65536);
		if(str.size()<256) return AddString1(str);
		else return AddString2(str);
	}

	void SendBuffer::AddString1(std::string str)
	{
		assert(str.size()<256);
		AddUByte(static_cast<ubyte>(str.size()));
		size_t len = buf.size();
		buf.resize(len+str.size());
		for(size_t i=0;i<str.size();i++)
		{
			buf[len+i] = static_cast<ubyte>(str[i]);
		}
	}

	void SendBuffer::AddString2(std::string str)
	{
		assert(str.size()<65536);
		AddUByte2(static_cast<ubyte2>(str.size()));
		size_t len = buf.size();
		buf.resize(len+str.size());
		for(size_t i=0;i<str.size();i++)
		{
			buf[len+i] = static_cast<ubyte>(str[i]);
		}
	}

	void SendBuffer::AddFloat1(float1 f)
	{
	}

	void SendBuffer::AddFloat2(float2 f)
	{
	}

			
	ENetPacket* SendBuffer::GetPacket(enet_uint32 flags)
	{
		ENetPacket* temp;
		ubyte* begin = &buf[0];
		void* vdata = static_cast<void*>(begin);
		temp = enet_packet_create(vdata, buf.size(), flags);
		return temp;
	}
	
	void SendBuffer::Send(ENetPeer* peer, enet_uint8 channelID, enet_uint32 flags)
	{
		ENetPacket* temp = GetPacket(flags);
		enet_peer_send(peer, channelID, temp);
	}
	
	void SendBuffer::Broadcast(ENetHost* host, enet_uint8 channelID, enet_uint32 flags)
	{
		ENetPacket* temp = GetPacket(flags);
		enet_host_broadcast(host, channelID, temp);
	}
	
	void SendBuffer::Clear()
	{
		buf.clear();
	}
	
	ReceiveBuffer::ReceiveBuffer(ENetPacket& packet)
	{
		data = static_cast<ubyte*>(packet.data);
		length = static_cast<size_t>(packet.dataLength);
		remaining = length;
	}
			
	ubyte ReceiveBuffer::GetUByte()
	{
		assert(remaining>=1);
		ubyte temp = data[length-remaining];
		remaining--;
		return temp;
	}
	
	sbyte ReceiveBuffer::GetByte()
	{
		assert(remaining>=1);
		sbyte temp = static_cast<sbyte>(data[length-remaining]);
		remaining--;
		return temp;
	}
	
	ubyte2 ReceiveBuffer::GetUByte2()
	{
		assert(remaining>=2);
		ubyte2* pos = reinterpret_cast<ubyte2*>(&data[length-remaining]);
		ubyte2 i = *pos;
		remaining-=2;
		return ntohs(i);
	}
	
	sbyte2 ReceiveBuffer::GetByte2()
	{
		assert(remaining>=2);
		ubyte2 u = GetUByte2();
		sbyte2 i = static_cast<sbyte2>(u);
		remaining-=2;
		return i;
	}
	
	ubyte4 ReceiveBuffer::GetUByte4()
	{
		assert(remaining>=4);
		ubyte4* pos = reinterpret_cast<ubyte4*>(&data[length-remaining]);
		ubyte4 i = *pos;
		remaining-=4;
		return ntohl(i);
	}
	
	sbyte4 ReceiveBuffer::GetByte4()
	{
		assert(remaining>=4);
		ubyte4 u = GetUByte4();
		sbyte4 i = static_cast<sbyte4>(u);
		remaining-=4;
		return i;
	}
	
	std::string ReceiveBuffer::GetString(size_t len)
	{
		std::string str = "";
		for(size_t i=0;i<len;i++)
		{
			str += static_cast<char>(GetByte());
		}
		return str;
	}
	
	std::string ReceiveBuffer::GetString1()
	{
		size_t len = static_cast<size_t>(GetUByte());
		return GetString(len);
	}
	
	std::string ReceiveBuffer::GetString2()
	{
		size_t len = static_cast<size_t>(GetUByte2());
		return GetString(len);
	}
			
	size_t ReceiveBuffer::Remaining() const
	{
		return remaining;
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
