SET(NETWORK_SRCS
	network/sore_network_buffers.cpp
	network/sore_network_common.cpp)
SET(NETWORK_HEADERS
	network/sore_network_buffers.h
	network/sore_network_common.h)
	
IF(UNIX)
	LIST(APPEND NETWORK_SRCS network/sore_network_common_linux.cpp)
ENDIF(UNIX)
IF(WIN32)
	LIST(APPEND NETWORK_SRCS network/sore_network_common_windows.cpp)
ENDIF(WIN32)

SET(SORE_NETWORK_SRCS ${NETWORK_SRCS} PARENT_SCOPE)
SET(SORE_NETWORK_HEADERS ${NETWORK_HEADERS} PARENT_SCOPE)