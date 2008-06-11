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

#ifndef  __SORE_GAMESTATE__
#define  __SORE_GAMESTATE__

#include <vector>
#include <string>
#include <map>
#ifdef _WIN32
#include <windows.h>
#endif
#include <enet/enet.h>

#include "sore_input.h"

//Type definitions for network transmission
typedef   signed char     sbyte;  //signed 8bit value
typedef unsigned char     ubyte;  //unsigned 8bit value
typedef   signed short    sbyte2; //signed 16bit value
typedef unsigned short    ubyte2; //unsigned 16bit value
typedef   signed int      sbyte4; //signed 32bit value
typedef unsigned int      ubyte4; //unsigned 32bit value

#ifdef _WIN32
typedef   signed __int64  sbyte8; //signed 64bit value
typedef unsigned __int64  ubyte8; //unsigned 64bit value
#else
typedef           int64_t sbyte8; //signed 64bit value
typedef          uint64_t ubyte8; //unsigned 64bit value
#endif

//single and double precision floats
typedef            float  float1;
typedef            double float2;

namespace SORE_Network
{
	const ubyte STATE_CONNECTING    = 0;
	const ubyte STATE_CONNECTED     = 1;
	const ubyte STATE_DISCONNECTING = 2;
	const ubyte STATE_OBSERVER      = 3;
	const ubyte STATE_PLAYER        = 4;
	
	const ubyte CHATMASK_ALL        = 0;
	const ubyte CHATMASK_TEAM       = 1;
	const ubyte CHATMASK_WHISPER    = 2;

	typedef std::vector<ubyte> net_buffer;
	
	struct player;
	
	typedef std::map<ubyte, player> player_list;
	typedef player_list::iterator player_ref;
	
	struct player
	{
		player() : name("unnamed"), playerState(STATE_CONNECTING), team(0), peer(NULL), player_ip(0) {player_ip_str[0]='\0';}
		std::string name;
		ubyte playerState;
		ubyte team; //implementation defined
		
		//only used on server
		player_ref id_iter; //retrieved from server
		ENetPeer* peer;
		
		//network info
		enet_uint32 player_ip;
		char player_ip_str[16];
	};
	
	ubyte8 pack754(long double f, unsigned bits, unsigned expbits);
	long double unpack754(ubyte8 i, unsigned bits, unsigned expbits);
	
	ubyte8 htonll(ubyte8 h);
	ubyte8 ntohll(ubyte8 n);
	
	class ReceiveBuffer
	{
		public:
			ReceiveBuffer(ENetPacket& packet);
			
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
		protected:
			ubyte* data;
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
			void Send(ENetPeer* peer, enet_uint8 channelID, enet_uint32 flags);
			void Broadcast(ENetHost* host, enet_uint8 channelID, enet_uint32 flags);
			void Clear();
			
			SendBuffer& operator+=(SendBuffer& b);
		protected:
			net_buffer buf;
	};
	
	class Gamestate;
	
	class GameInput
	{
		//TODO: Add some sort of method to transform real input -> GameInput. Should be flexible enough to allow for different settings, etc
		public:
			GameInput() {}
					
			virtual ~GameInput() {}
			
			virtual void Serialize(SendBuffer& send) = 0; //shove this into send
			virtual void SetGamestate(Gamestate* game) = 0;
			virtual bool RequestUpdate() = 0;
			
		protected:
			virtual void Deserialize(ReceiveBuffer& receive) = 0; //deserializes into this
	};
	
	typedef unsigned int gamestate_diff;
	
	const gamestate_diff NO_DIFFERENCE        = 0;
	const gamestate_diff DEVIATION_DIFFERENCE = 1;
	const gamestate_diff SEVERE_DIFFERENCE    = 2;
	
	class Gamestate
	{
		public:
			virtual ~Gamestate() {}
			
			//TODO: Gatestate needs a time signal - contained in GameInput, or separately to Simulate? also, non-player-influenced actions should be dealt with somehow...
			
			virtual void SimulateInput(GameInput* input) = 0;
			virtual void SimulateTime(unsigned int ticks) = 0; //tick = 1/10000 sec
			
			virtual void Delta(Gamestate* old, SendBuffer& send) = 0;
			virtual void Serialize(SendBuffer& send) = 0;
			virtual void Deserialize(ReceiveBuffer& receive) = 0; //capable of deserializing deltas/full transfers
			
			virtual gamestate_diff Difference(Gamestate* old) = 0; //returns DEIVIATION is this and old are different enough to warrant sending a correction packet, SEVERE if host must send complete gamestate, NO if no action needed
	};
	
	class GamestateFactory
	{
		public:
			virtual ~GamestateFactory() {}
			
			virtual Gamestate* CreateGamestate() = 0;
			virtual Gamestate* CreateGamestate(Gamestate* copy) = 0; //copy constructor
			virtual GameInput* CreateGameInput() = 0;
			virtual GameInput* CreateGameInput(ReceiveBuffer& receive) = 0;
	};
}

#endif /*__SORE_GAMESTATE__*/
