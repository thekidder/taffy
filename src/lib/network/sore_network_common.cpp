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

#include <cassert>
#include <boost/format.hpp>
#include <enet/enet.h>

#include <sore_logger.h>
#include <sore_network_common.h>

namespace SORE_Network
{
	ubyte8 htonll(ubyte8 h)
	{
		ubyte8 n;
		ubyte* ptr = reinterpret_cast<ubyte*>(&n);
		*ptr++ = static_cast<ubyte>(h>>56);
		*ptr++ = static_cast<ubyte>(h>>48);
		*ptr++ = static_cast<ubyte>(h>>40);
		*ptr++ = static_cast<ubyte>(h>>32);
		*ptr++ = static_cast<ubyte>(h>>24);
		*ptr++ = static_cast<ubyte>(h>>16);
		*ptr++ = static_cast<ubyte>(h>>8);
		*ptr++ = static_cast<ubyte>(h);
		return n;
	}
	
	ubyte8 ntohll(ubyte8 n)
	{
		ubyte* ptr = reinterpret_cast<ubyte*>(&n);
		return (static_cast<ubyte8>(ptr[0])<<56) | (static_cast<ubyte8>(ptr[1])<<48) | (static_cast<ubyte8>(ptr[2])<<40) | 
			(static_cast<ubyte8>(ptr[3])<<32) | (static_cast<ubyte8>(ptr[4])<<24) | (static_cast<ubyte8>(ptr[5])<<16) |
			(static_cast<ubyte8>(ptr[6])<<8) | (static_cast<ubyte8>(ptr[7]));
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
		result = static_cast<long double>(i&((static_cast<ubyte8>(1)<<significandbits)-1)); // mask
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

	std::string AddrToStr(ENetAddress& addr)
	{
		char host[256];
		enet_address_get_host_ip(&addr, host, 255);
		return boost::str(boost::format("%s:%d") % host % addr.port);
	}

	static bool network_ok = false;
	
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
}
