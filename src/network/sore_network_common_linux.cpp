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
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <enet/enet.h>

#include "../sore_logger.h"
#include "sore_network_common.h"

namespace SORE_Network
{
	std::vector<ENetAddress> GetHostAddresses()
	{
		const unsigned int len = 1024;

		std::vector<ENetAddress> addresses;
		char hostname[len];// = "localhost";
		gethostname(hostname, len);

		addrinfo hints, *result;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_protocol = IPPROTO_UDP;
		ENGINE_LOG(SORE_Logging::LVL_DEBUG2, std::string("Hostname is ") + hostname);

		int error = getaddrinfo(hostname, NULL, &hints, &result);
		//hostent* h = gethostbyname(hostname);

		//in_addr** addr_list = reinterpret_cast<in_addr**>(h->h_addr_list);

		//for(int i=0; addr_list[i] != NULL; ++i)
		for(addrinfo* i=result; i!=NULL; i = i->ai_next)
		{
			if(i->ai_addr->sa_family == AF_INET)
			{
				sockaddr_in* socket = reinterpret_cast<sockaddr_in*>(i->ai_addr);

				//in_addr* addr = addr_list[i];

				ENetAddress temp;
				temp.host = socket->sin_addr.s_addr;//addr->s_addr
				addresses.push_back(temp);

				//char* ip = inet_ntoa(*addr);
				//ENGINE_LOG(SORE_Logging::LVL_DEBUG2, std::string("IP is: ") + ip);

				//char hostname[1025];
				//getnameinfo(i->ai_addr, i->ai_addrlen, hostname, 1025, NULL, 0, NI_NUMERICHOST);
				//if(*hostname)
				//	ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Found hostname: %s") % hostname);
			}
		}

		freeaddrinfo(result);

		return addresses;
	}
}
