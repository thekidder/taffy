cmake_minimum_required(VERSION 2.6)

set(SORE_NETWORK_SRCS
	network/sore_network_broadcaster.cpp network/sore_network_buffers.cpp network/sore_network_client.cpp 
	network/sore_network_common.cpp network/sore_network_player.cpp 
	network/sore_network_server.cpp
	PARENT_SCOPE)
set(SORE_NETWORK_HEADERS
	network/sore_network_broadcaster.h network/sore_network_buffers.h network/sore_network_client.h network/sore_network_common.h
	network/sore_network_game.h network/sore_network_player.h	network/sore_network_server.h
	PARENT_SCOPE)
	
IF(UNIX)
set(SORE_PLATFORM_SPECIFIC ${SORE_PLATFORM_SPECIFIC} network/sore_network_common_linux.cpp PARENT_SCOPE)
ENDIF(UNIX)
IF(WIN32)
set(SORE_PLATFORM_SPECIFIC ${SORE_PLATFORM_SPECIFIC} PARENT_SCOPE)
ENDIF(WIN32)