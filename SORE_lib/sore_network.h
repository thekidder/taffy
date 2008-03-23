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

#ifndef  __SORE_NETWORK_H__
#define  __SORE_NETWORK_H__

#include "sore_task.h"
#include "sore_settings.h"
#include "sore_defines.h"
#include <enet/enet.h>

/*
 * This subsystem is experimental. It provides a testing ground for networking
 * additions to the SORE library.
 * Currently SORE_Network provides Server and Client classes, which are used for
 * UDP/LAN connections and management. This is not permanent and may change at
 * any time.
 */
 
namespace SORE_Network
{
	void InitNetwork();
	
	/*class NetworkConnection : public SORE_Kernel::Task
	{
		public:
			NetworkConnection::NetworkConnection(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm);
			NetworkConnection::~NetworkConnection();
			
			void Frame(int elapsed);
			const char* GetName() const {return "SORE Networking connection task";}
		protected:
			ENetHost* host;
			SORE_Utility::SettingsManager sm;
	};*/
	
	class Server : public SORE_Kernel::Task
	{
		public:
			Server(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm);
			~Server();
			
			void Frame(int elapsed);
			const char* GetName() const {return "SORE Networking server task";}
		protected:
			ENetHost* server;
			SORE_Utility::SettingsManager sm;
			ENetSocket sock;
	};
	
	class Client : public SORE_Kernel::Task
	{
		public:
			Client(SORE_Kernel::GameKernel* gk, SORE_Utility::SettingsManager& _sm);
			~Client();
			
			void Frame(int elapsed);
			const char* GetName() const {return "SORE Networking client task";}
		protected:
			ENetHost* client;
			ENetPeer* server;
			SORE_Utility::SettingsManager sm;
			ENetSocket sock;
	};
}

#endif /*__SORE_NETWORK_H__*/
