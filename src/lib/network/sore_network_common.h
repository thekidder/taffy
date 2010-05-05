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
