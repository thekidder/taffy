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
// $Id$

#ifndef SORE_NETWORK_BROADCASTER_H
#define SORE_NETWORK_BROADCASTER_H

#include <boost/function.hpp>
#include <enet/enet.h>

#include "../sore_task.h"

namespace SORE_Network
{
	class Server;
	
	class UDPBroadcaster : public SORE_Kernel::Task
	{
		public:
			UDPBroadcaster(ENetAddress broadcastAddress, boost::function<ENetBuffer (Server*)> c);
			~UDPBroadcaster();
			
			void SetBroadcastAddress(ENetAddress broadcastAddress);
			void SetBroadcastCallback(boost::function<ENetBuffer (Server*)> c);
			void SetServer(Server* s);
			
			void Frame(int elapsed);
			const char* GetName() const {return "UDP Broadcaster";}
		protected:
			ENetSocket broadcaster;
			ENetAddress address;
			boost::function<ENetBuffer (Server*)> callback;
			Server* serv;
	};
}

#endif
