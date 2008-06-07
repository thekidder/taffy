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

#include <algorithm>

/*bool operator<(ENetAddress a, ENetAddress b)
{
	if(a.host<b.host) return true;
	if(a.port<b.port) return true;
	return false;
}*/

namespace SORE_Network
{
	bool network_ok = false;
	
	void InitNetwork()
	{
		assert(sizeof(ubyte )==1 && sizeof(sbyte )==1 && "Byte type is not of the correct size");
		assert(sizeof(ubyte2)==2 && sizeof(sbyte2)==2 && "Byte2 type is not of the correct size");
		assert(sizeof(ubyte4)==4 && sizeof(sbyte4)==4 && "Byte4 type is not of the correct size");
		assert(sizeof(ubyte8)==8 && sizeof(sbyte8)==8 && "Byte8 type is not of the correct size");
		
		assert(sizeof(float1)==4 && sizeof(float2)==8 && "Float types are not of the correct size");
		
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
	
	ubyte8 htonll(ubyte8 h)
	{
		ubyte8 n;
		ubyte* ptr = reinterpret_cast<ubyte*>(&n);
		*ptr++ = h>>56;
		*ptr++ = h>>48;
		*ptr++ = h>>40;
		*ptr++ = h>>32;
		*ptr++ = h>>24;
		*ptr++ = h>>16;
		*ptr++ = h>>8;
		*ptr++ = h;
		return n;
	}
	
	ubyte8 ntohll(ubyte8 n)
	{
		ubyte* ptr = reinterpret_cast<ubyte*>(&n);
		return (static_cast<ubyte8>(ptr[0])<<56) | (static_cast<ubyte8>(ptr[1])<<48) | (static_cast<ubyte8>(ptr[2])<<40) | (static_cast<ubyte8>(ptr[3])<<32) | (static_cast<ubyte8>(ptr[4])<<24) | (static_cast<ubyte8>(ptr[5])<<16) | (static_cast<ubyte8>(ptr[6])<<8) | (static_cast<ubyte8>(ptr[7]));
	}
	
	ubyte8 pack754(long double f, unsigned bits, unsigned expbits)
	{
		long double fnorm;
		int shift;
		ubyte8 sign, exp, significand;
		unsigned significandbits = bits - expbits - 1; // -1 for sign bit

		if (f == 0.0) return 0; // get this special case out of the way

    // check sign and begin normalization
		if (f < 0) { sign = 1; fnorm = -f; }
		else { sign = 0; fnorm = f; }

    // get the normalized form of f and track the exponent
		shift = 0;
		while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
		while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
		fnorm = fnorm - 1.0;

    // calculate the binary form (non-float) of the significand data
		significand = static_cast<ubyte8>(fnorm * ((static_cast<ubyte8>(1)<<significandbits) + 0.5f));

    // get the biased exponent
		exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

    // return the final answer
		return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
	}

	long double unpack754(ubyte8 i, unsigned bits, unsigned expbits)
	{
		long double result;
		ubyte8 shift;
		unsigned bias;
		unsigned significandbits = bits - expbits - 1; // -1 for sign bit

		if (i == 0) return 0.0;

    // pull the significand
		result = (i&((static_cast<ubyte8>(1)<<significandbits)-1)); // mask
		result /= (static_cast<ubyte8>(1)<<significandbits); // convert back to float
		result += 1.0f; // add the one back on

    // deal with the exponent
		bias = (1<<(expbits-1)) - 1;
		shift = ((i>>significandbits)&((static_cast<ubyte8>(1)<<expbits)-1)) - bias;
		while(shift > 0) { result *= 2.0; shift--; }
		while(shift < 0) { result /= 2.0; shift++; }

    // sign it
		result *= (i>>(bits-1))&1? -1.0: 1.0;

		return result;
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
	
	void SendBuffer::AddUByte8(ubyte8 b)
	{
		ubyte8 n = htonll(b);
		size_t len = buf.size();
		buf.resize(len+8);
		ubyte8* ptr = reinterpret_cast<ubyte8*>(&buf[len]);
		*ptr = n;
	}

	void SendBuffer::AddByte8 (sbyte8 b)
	{
		AddUByte8(static_cast<ubyte8>(b));
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

	void SendBuffer::AddFloat(float1 f, size_t numBytes)
	{
		return AddFloat(static_cast<float2>(f), numBytes);
	}

	void SendBuffer::AddFloat(float2 f, size_t numBytes)
	{
		unsigned int bits, expbits;
		switch(numBytes)
		{
			case 1:
			{
				bits = 8;
				expbits = 2;
				ubyte packed = pack754(f, bits, expbits);
				AddUByte2(packed);
				break;
			}
			case 2:
			{
				bits = 16;
				expbits = 4;
				ubyte2 packed = pack754(f, bits, expbits);
				AddUByte2(packed);
				break;
			}
			case 4:
			{
				/*bits = 32;
				expbits = 8;
				ubyte4 packed = pack754(f, bits, expbits);
				AddUByte4(packed);*/
				
				size_t len = buf.size();
				buf.resize(len+4);
				
				float1* ptr = reinterpret_cast<float1*>(&buf[len]);
				*ptr = static_cast<float1>(f);
				break;
			}
			case 8:
			{
				/*bits = 64;
				expbits = 11;
				ubyte8 packed = pack754(f, bits, expbits);
				//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("adding encoded double: 0x%016llX") % packed);
				AddUByte8(packed);*/
				
				size_t len = buf.size();
				buf.resize(len+8);
				
				float2* ptr = reinterpret_cast<float2*>(&buf[len]);
				*ptr = f;
				break;
			}
			default:
				ENGINE_LOG(SORE_Logging::LVL_WARNING, "Unsupported double size");
		}
		
	}

	SendBuffer& SendBuffer::operator+=(SendBuffer& b)
	{
		//net_buffer::iterator it = buf.end();
		//buf.resize(buf.size() + b.buf.size());
		//std::copy(b.buf.begin(), b.buf.end(), it);
		buf.insert(buf.end(), b.buf.begin(), b.buf.end());
		return *this;
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
		return i;
	}
	
	ubyte8 ReceiveBuffer::GetUByte8()
	{
		assert(remaining>=8);
		ubyte8* pos = reinterpret_cast<ubyte8*>(&data[length-remaining]);
		ubyte8 i = *pos;
		remaining-=8;
		return ntohll(i);
	}
	
	sbyte8 ReceiveBuffer::GetByte8()
	{
		assert(remaining>=8);
		ubyte8 u = GetUByte8();
		sbyte8 i = static_cast<sbyte8>(u);
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
	
	double ReceiveBuffer::GetFloat(size_t numBytes)
	{
		assert(numBytes==1 || numBytes==2 || numBytes==4 || numBytes==8);
		ubyte8 num;
		unsigned int bits, expbits;
		switch(numBytes)
		{
			case 1:
				num = static_cast<ubyte8>(GetUByte());
				bits = 8;
				expbits = 2;
				break;
			case 2:
				num = static_cast<ubyte8>(GetUByte2());
				bits = 16;
				expbits = 4;
				break;
			case 4:
			{
				/*num = static_cast<ubyte8>(GetUByte4());
				bits = 32;
				expbits = 8;*/
				assert(remaining>=4);
				float1* pos = reinterpret_cast<float1*>(&data[length-remaining]);
				float1 i = *pos;
				remaining-=4;
				return static_cast<float2>(i);
				break;
			}
			case 8:
			{
				/*num = static_cast<ubyte8>(GetUByte8());
				//ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("receiving encoded double: 0x%016llX") % num);
				bits = 64;
				expbits = 11;*/
				assert(remaining>=8);
				float2* pos = reinterpret_cast<float2*>(&data[length-remaining]);
				float2 i = *pos;
				remaining-=8;
				return i;
				break;
			}
			default:
				ENGINE_LOG(SORE_Logging::LVL_WARNING, "Unsupported double size");
		}
		return static_cast<double>(unpack754(num, bits, expbits));
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
	
	/*SyncedDataBase::SyncedDataBase(SyncedDataCollection& collection)
	{
		collection.Add(this);
	}

	bool SyncedDataBase::Changed()
	{
		return changed;
	}
	
	void SyncedDataCollection::Add(SyncedDataBase* datum)
	{
		data.push_back(datum);
	}
	
	void SyncedDataCollection::ReceiveCallback(ReceiveBuffer& receive)
	{
		size_t numItems = data.size();
		//first we read the number of items to update
		unsigned int num;
		if(numItems<256)
			num = static_cast<unsigned int>(receive.GetUByte());
		else if(numItems<65535)
			num = static_cast<unsigned int>(receive.GetUByte2());
		else
			num = static_cast<unsigned int>(receive.GetUByte4());
		
		for(int i=0;i<num;++i)
		{
			//get index of item to update
			size_t index;
			if(numItems<256)
				index = static_cast<size_t>(receive.GetUByte());
			else if(numItems<65535)
				index = static_cast<size_t>(receive.GetUByte2());
			else
				index = static_cast<size_t>(receive.GetUByte4());
			
			//Deserialize(data[index]->get(), receive);
			data[index]->Receive(receive);
		}
	}
	
	void SyncedDataCollection::Send(SendBuffer& send)
	{
		size_t numItems = data.size();
		unsigned int toUpdate = 0;
		for(std::vector<SyncedDataBase*>::iterator it=data.begin();it!=data.end();++it)
		{
			if((*it)->Changed()) ++toUpdate;
		}
		if(numItems<256)
			send.AddUByte(static_cast<ubyte>(toUpdate));
		else if(numItems<65535)
			send.AddUByte2(static_cast<ubyte2>(toUpdate));
		else
			send.AddUByte4(static_cast<ubyte4>(toUpdate));
		
		for(size_t i=0;i<data.size();++i)
		{
			if(data[i]->Changed())
			{
				if(numItems<256)
					send.AddUByte(static_cast<ubyte>(i));
				else if(numItems<65535)
					send.AddUByte2(static_cast<ubyte2>(i));
				else
					send.AddUByte4(static_cast<ubyte4>(i));
				data[i]->Send(send);
			}
		}
	}
	
	bool SyncedDataCollection::Updated() const
	{
		for(std::vector<SyncedDataBase*>::const_iterator it=data.begin();it!=data.end();++it)
		{
			if((*it)->Changed()) return true;
		}
		return false;
	}*/
}
