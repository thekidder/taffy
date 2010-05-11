# Try to find SORE
# This will define:
#
# SORE_FOUND
# SORE_INCLUDE_DIR - include directory
# SORE_LIBRARIES - libraries to link
# SORE_DLL - set to the DLL path if on Windows and building a shared library
# 

FIND_PATH(INCLUDE_DIR sore/sore_defines.h
    PATHS
    /usr/include
    /usr/local/include
)

FIND_LIBRARY(SORE_LIBRARY_RELEASE
    NAMES sore
    PATHS
    PATH_SUFFIXES lib64 lib
    /usr/local
    /usr
)

FIND_LIBRARY(SORE_LIBRARY_DEBUG
    NAMES sore_debug
    PATHS
    PATH_SUFFIXES lib64 lib
    /usr/local
    /usr
)

IF(WIN32)
	FIND_FILE(SORE_DLL_RELEASE
		NAMES bin/sore.dll
	)

	FIND_FILE(SORE_DLL_DEBUG
		NAMES bin/sore_debug.dll
	)
ENDIF()

IF(INCLUDE_DIR)
	SET(SORE_INCLUDE_DIR ${INCLUDE_DIR}/sore)
ENDIF()
IF(SORE_LIBRARY_DEBUG AND NOT SORE_LIBRARY_RELEASE)
    SET(SORE_LIBRARY ${SORE_LIBRARY_DEBUG})
ELSEIF(SORE_LIBRARY_RELEASE AND NOT SORE_LIBRARY_DEBUG)
    SET(SORE_LIBRARY ${SORE_LIBRARY_RELEASE})
ELSE()
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
        SET(SORE_LIBRARY optimized ${SORE_LIBRARY_RELEASE} debug ${SORE_LIBRARY_DEBUG})
    ELSE()
        # if there are no configuration types and CMAKE_BUILD_TYPE has no value
        # then just use the release libraries
        SET(SORE_LIBRARY ${SORE_LIBRARY_RELEASE})
    ENDIF()
ENDIF()

IF(WIN32)
	IF(SORE_DLL_DEBUG AND NOT SORE_DLL_RELEASE)
		SET(SORE_DLL ${SORE_DLL_DEBUG})
	ELSEIF(SORE_DLL_RELEASE AND NOT SORE_DLL_DEBUG)
		SET(SORE_DLL ${SORE_DLL_RELEASE})
	ELSE()
		IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
			SET(SORE_DLL optimized ${SORE_DLL_RELEASE} debug ${SORE_DLL_DEBUG})
		ELSE()
			# if there are no configuration types and CMAKE_BUILD_TYPE has no value
			# then just use the release libraries
			SET(SORE_DLL ${SORE_DLL_RELEASE})
		ENDIF()
	ENDIF()
ENDIF()

SET(SORE_FOUND FALSE)
IF (SORE_INCLUDE_DIR AND SORE_LIBRARY)
    SET(SORE_FOUND TRUE)

    SET(SORE_LIBRARIES ${SORE_LIBRARY})
ENDIF (SORE_INCLUDE_DIR AND SORE_LIBRARY)

IF (SORE_FOUND)
    MESSAGE(STATUS "SORE was found.")
        MESSAGE (STATUS "  include path: ${SORE_INCLUDE_DIR}")
        MESSAGE (STATUS "  library path: ${SORE_LIBRARY}")
	IF(WIN32)
		MESSAGE (STATUS "  DLL path: ${SORE_DLL}")
	ENDIF()
ELSE (SORE_FOUND)
    IF (NOT SORE_INCLUDE_DIR)
        MESSAGE(SEND_ERROR "SORE include path was not found.")
    ENDIF (NOT SORE_INCLUDE_DIR)
    IF (NOT SORE_LIBRARY)
        MESSAGE(SEND_ERROR "SORE library was not found.")
    ENDIF (NOT SORE_LIBRARY)
ENDIF (SORE_FOUND)