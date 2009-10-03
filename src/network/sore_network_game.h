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

#ifndef SORE_NETWORK_GAME_H
#define SORE_NETWORK_GAME_H

#include "sore_network_player.h"

namespace SORE_Network
{
	class Game;
	
	class GameInput
	{
		//TODO: Add some sort of method to transform real input -> GameInput. Should be flexible enough to allow for different settings, etc
		public:
			GameInput() {}
					
			virtual ~GameInput() {}
			
			virtual void Serialize(SendBuffer& send) = 0; //shove this into send
			virtual void SetGame(Game* game) = 0;
			virtual bool RequestUpdate() = 0;
			
		protected:
			virtual void Deserialize(ReceiveBuffer& receive) = 0; //deserializes into this
	};
	
	typedef unsigned int gamestate_diff;
	
	const gamestate_diff NO_DIFFERENCE        = 0;
	const gamestate_diff DEVIATION_DIFFERENCE = 1;
	const gamestate_diff SEVERE_DIFFERENCE    = 2;
	
	class Game
	{
		public:
			virtual ~Game() {}
			
			//TODO: Gatestate needs a time signal - contained in GameInput, or separately to Simulate? also, non-player-influenced actions should be dealt with somehow...
			
			virtual void SimulateInput(ubyte player, GameInput* input) = 0;
			virtual void SimulateTime(unsigned int ticks) = 0; //tick = 1/10000 sec
			
			virtual void Delta(Game* old, SendBuffer& send) = 0;
			virtual void Serialize(SendBuffer& send) = 0;
			virtual void Deserialize(ReceiveBuffer& receive) = 0; //capable of deserializing deltas/full transfers
			virtual bool ForceCompleteUpdate() = 0;
			
			//server notification functions
			virtual void OnPlayerStateChange(player_ref player, ubyte oldState) {}
			
			virtual gamestate_diff Difference(Game* old) = 0; //returns DEVIATION is this and old are different enough to warrant sending a correction packet, SEVERE if host must send complete gamestate, NO if no action needed
			
			virtual std::string NetworkVersion() = 0;
	};
	
	class GameFactory
	{
		public:
			virtual ~GameFactory() {}
			
			virtual Game* CreateGame() = 0;
			virtual Game* CreateGame(Game* copy) = 0; //copy constructor
			virtual GameInput* CreateGameInput() = 0;
			virtual GameInput* CreateGameInput(ReceiveBuffer& receive) = 0;
	};
}

#endif
