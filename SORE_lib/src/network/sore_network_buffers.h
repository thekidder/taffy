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
			ubyte  GetUByte();
			sbyte  GetByte();
			ubyte2 GetUByte2();
			sbyte2 GetByte2();
			ubyte4 GetUByte4();
			sbyte4 GetByte4();
			ubyte8 GetUByte8();
			sbyte8 GetByte8();
			 
			 //conversions needed
			std::string GetString(size_t len); //string with given length
			std::string GetString1(); //string with one-byte (unsigned) preceding length
			std::string GetString2(); //string with two-byte (unsigned) preceding length
			
			double GetFloat(size_t numBytes);
			
			size_t Remaining() const;
			
		private:
			ubyte* data;
			std::vector<ubyte> ownData;
			size_t length;
			size_t remaining;
	};
	
	class SendBuffer
	{
		public:
			SendBuffer();
			
			void AddUByte (ubyte b);
			void AddByte  (sbyte b);
			
			void AddUByte2(ubyte2 b);
			void AddByte2 (sbyte2 b);
			
			void AddUByte4(ubyte4 b);
			void AddByte4 (sbyte4 b);
			
			void AddUByte8(ubyte8 b);
			void AddByte8 (sbyte8 b);
			
			void AddString(std::string str);
			void AddString1(std::string str);
			void AddString2(std::string str);
			
			void AddFloat(float1 f, size_t numBytes);
			void AddFloat(float2 f, size_t numBytes);
			
			void AddFloat(float1 f); //default precision: 4 bytes
			void AddFloat(float2 f); //default precision: 8 bytes
			
			ENetPacket* GetPacket(enet_uint32 flags);
			unsigned int size() const;
			void Send(ENetPeer* peer, enet_uint8 channelID, enet_uint32 flags);
			void Broadcast(ENetHost* host, enet_uint8 channelID, enet_uint32 flags);
			void Clear();
			
			static unsigned int GetTotalBytes() { return totalBytesSent;}
			static void         ResetTotalBytes() { totalBytesSent = 0u; }
			
			SendBuffer& operator+=(SendBuffer& b);
			
			void Compress(SendBuffer& compressed); //compresses into argument
		protected:
			net_buffer buf;
			
			static unsigned int totalBytesSent;
	};
}

#endif
