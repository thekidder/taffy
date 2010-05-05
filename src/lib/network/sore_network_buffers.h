/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#ifndef SORE_NETWORK_BUFFERS_H
#define SORE_NETWORK_BUFFERS_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <vector>
#include <string>
#ifdef _WIN32
#include <sore_defines.h> //for windows.h
#endif
#include <enet/enet.h>

#include <sore_network_common.h>

namespace SORE_Network
{
	typedef std::vector<ubyte> net_buffer;

	class SORE_EXPORT ReceiveBuffer
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
	
	class SORE_EXPORT SendBuffer
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

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
