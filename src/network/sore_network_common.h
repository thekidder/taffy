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

#ifndef  SORE_NETWORK_COMMON_H
#define  SORE_NETWORK_COMMON_H

#include <string>
#include <vector>
#include <boost/cstdint.hpp>
#include <enet/enet.h>

//Type definitions for network transmission
typedef boost::int_least8_t   sbyte;  //signed 8bit value
typedef boost::uint_least8_t  ubyte;  //unsigned 8bit value
typedef boost::int_least16_t  sbyte2; //signed 16bit value
typedef boost::uint_least16_t ubyte2; //unsigned 16bit value
typedef boost::int_least32_t  sbyte4; //signed 32bit value
typedef boost::uint_least32_t ubyte4; //unsigned 32bit value
typedef boost::int_least64_t  sbyte8; //signed 64bit value
typedef boost::uint_least64_t ubyte8; //unsigned 64bit value

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
	
	ubyte8 pack754(long double f, unsigned bits, unsigned expbits);
	long double unpack754(ubyte8 i, unsigned bits, unsigned expbits);
	
	ubyte8 htonll(ubyte8 h);
	ubyte8 ntohll(ubyte8 n);

	std::string AddrToStr(ENetAddress& addr);

	//call before any network calls - initializes ENet
	void InitNetwork();

	std::vector<ENetAddress> GetHostAddresses();
}

#endif
