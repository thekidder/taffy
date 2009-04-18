/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
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

#ifndef SORE_NETWORK_BUFFERS_H
#define SORE_NETWORK_BUFFERS_H

#include <vector>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
#include <enet/enet.h>

#include "sore_network_common.h"

namespace SORE_Network
{
	typedef std::vector<ubyte> net_buffer;

	class ReceiveBuffer
	{
		public:
			ReceiveBuffer(ENetPacket& packet);
			ReceiveBuffer(ReceiveBuffer& b, bool compressed = false);
			~ReceiveBuffer();
			
			//primitives
			ReceiveBuffer& operator>>(ubyte & data);
			ReceiveBuffer& operator>>(sbyte & data);
			ReceiveBuffer& operator>>(ubyte2& data);
			ReceiveBuffer& operator>>(sbyte2& data);
			ReceiveBuffer& operator>>(ubyte4& data);
			ReceiveBuffer& operator>>(sbyte4& data);
			ReceiveBuffer& operator>>(ubyte8& data);
			ReceiveBuffer& operator>>(sbyte8& data);

			ReceiveBuffer& operator>>(std::string& data);
	
			ReceiveBuffer& operator>>(float1& data);
			ReceiveBuffer& operator>>(float2& data);

			ReceiveBuffer& operator&(ubyte & data);
			ReceiveBuffer& operator&(sbyte & data);
			ReceiveBuffer& operator&(ubyte2& data);
			ReceiveBuffer& operator&(sbyte2& data);
			ReceiveBuffer& operator&(ubyte4& data);
			ReceiveBuffer& operator&(sbyte4& data);
			ReceiveBuffer& operator&(ubyte8& data);
			ReceiveBuffer& operator&(sbyte8& data);

			ReceiveBuffer& operator&(std::string& data);
			
			ReceiveBuffer& operator&(float1& data);
			ReceiveBuffer& operator&(float2& data);
			
			size_t Remaining() const;
			
		private:
			float2 GetFloat(size_t numBytes);

			ubyte* buf;
			std::vector<ubyte> ownData;
			size_t length;
			size_t remaining;
	};
	
	class SendBuffer
	{
		public:
			SendBuffer();
			
			SendBuffer& operator<<(const ubyte & data);
			SendBuffer& operator<<(const sbyte & data);
			SendBuffer& operator<<(const ubyte2& data);
			SendBuffer& operator<<(const sbyte2& data);
			SendBuffer& operator<<(const ubyte4& data);
			SendBuffer& operator<<(const sbyte4& data);
			SendBuffer& operator<<(const ubyte8& data);
			SendBuffer& operator<<(const sbyte8& data);

			SendBuffer& operator<<(const std::string& data);
	
			SendBuffer& operator<<(const float1& data);
			SendBuffer& operator<<(const float2& data);

			SendBuffer& operator&(const ubyte & data);
			SendBuffer& operator&(const sbyte & data);
			SendBuffer& operator&(const ubyte2& data);
			SendBuffer& operator&(const sbyte2& data);
			SendBuffer& operator&(const ubyte4& data);
			SendBuffer& operator&(const sbyte4& data);
			SendBuffer& operator&(const ubyte8& data);
			SendBuffer& operator&(const sbyte8& data);

			SendBuffer& operator&(const std::string& data);
			
			SendBuffer& operator&(const float1& data);
			SendBuffer& operator&(const float2& data);
			
			ENetPacket* GetPacket(enet_uint32 flags);
			unsigned int size() const;
			void Send(ENetPeer* peer, enet_uint8 channelID, enet_uint32 flags);
			void Broadcast(ENetHost* host, enet_uint8 channelID, enet_uint32 flags);
			void Clear();
			
			static unsigned int GetTotalBytes();
			static void         ResetTotalBytes();
			
			SendBuffer& operator+=(SendBuffer& b);
			
			void Compress(SendBuffer& compressed); //compresses into argument
		private:
			void AddFloat(float2 f, size_t numBytes);

			net_buffer buf;
			static unsigned int totalBytesSent;
	};
}

#endif
