# - Try to find enet
# Once done this will define
#
#  ENET_FOUND - system has enet
#  ENET_INCLUDE_DIR - the enet include directory
#  ENET_LIBRARIES - the libraries needed to use enet
#  ENET_DEFINITIONS - Compiler switches required for using enet
#
#  Borrowed from The Mana World
#  http://themanaworld.org/
#
# Several changes and additions by Fabian 'x3n' Landau
#                 > www.orxonox.net <

IF (ENet_INCLUDE_DIR AND ENet_LIBRARY)
   SET(ENet_FIND_QUIETLY TRUE)
ENDIF (ENet_INCLUDE_DIR AND ENet_LIBRARY)

FIND_PATH(ENet_INCLUDE_DIR enet/enet.h
    PATHS
    /usr/include
    /usr/local/include
    /usr/pack/enet-2007-sd/include
    ../libs/enet-1.1/include
    ${DEPENDENCY_DIR}/enet-1.2/include
	C:\\DEVEL\\include
    )

FIND_LIBRARY(ENet_LIBRARY
    NAMES enet
    HINTS
    $ENV{ENETDIR}
    PATH_SUFFIXES lib64 lib
    PATHS
    /usr/local
    /usr
    )

SET(ENET_FOUND FALSE)
IF (ENet_INCLUDE_DIR AND ENet_LIBRARY)
    SET(ENET_FOUND TRUE)
    SET(ENET_INCLUDE_DIR ${ENet_INCLUDE_DIR})

    IF(WIN32)
        SET(WINDOWS_ENET_DEPENDENCIES "ws2_32;winmm")
        SET(ENet_LIBRARY ${ENet_LIBRARY} ${WINDOWS_ENET_DEPENDENCIES})
    ENDIF(WIN32)

    SET(ENET_LIBRARIES ${ENet_LIBRARY})
ENDIF (ENet_INCLUDE_DIR AND ENet_LIBRARY)

IF (ENET_FOUND)
    #IF (NOT ENet_FIND_QUIETLY)
        MESSAGE(STATUS "ENet was found.")
        #IF (VERBOSE_FIND)
            MESSAGE (STATUS "  include path: ${ENet_INCLUDE_DIR}")
            MESSAGE (STATUS "  library path: ${ENet_LIBRARY}")
            MESSAGE (STATUS "  libraries:    enet")
        #ENDIF (VERBOSE_FIND)
    #ENDIF (NOT ENet_FIND_QUIETLY)
ELSE (ENET_FOUND)
    IF (NOT ENet_INCLUDE_DIR)
        MESSAGE(SEND_ERROR "ENet include path was not found.")
    ENDIF (NOT ENet_INCLUDE_DIR)
    IF (NOT ENet_LIBRARY)
        MESSAGE(SEND_ERROR "ENet library was not found.")
    ENDIF (NOT ENet_LIBRARY)
ENDIF (ENET_FOUND)

MARK_AS_ADVANCED(ENet_INCLUDE_DIR ENet_LIBRARY)