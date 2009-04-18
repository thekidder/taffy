cmake_minimum_required(VERSION 2.6)

set(SORE_NETWORK_SRCS
	network/sore_network_buffers.cpp
	network/sore_network_common.cpp
	PARENT_SCOPE)
set(SORE_NETWORK_HEADERS
	network/sore_network_buffers.h
	network/sore_network_common.h
	PARENT_SCOPE)
	
IF(UNIX)
set(SORE_PLATFORM_SPECIFIC ${SORE_PLATFORM_SPECIFIC} network/sore_network_common_linux.cpp PARENT_SCOPE)
ENDIF(UNIX)
IF(WIN32)
set(SORE_PLATFORM_SPECIFIC ${SORE_PLATFORM_SPECIFIC} network/sore_network_common_windows.cpp PARENT_SCOPE)
ENDIF(WIN32)