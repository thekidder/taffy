OPTION(SORE_WITH_VBO "Build with support for VBOs instead of vertex arrays" ON)
OPTION(SORE_WITH_ENET "Build with ENet networking support" OFF)

IF(WIN32)
	SET(Boost_USE_STATIC_LIBS ON)
	SET(Boost_USE_MULTITHREADED ON)
ENDIF()

FIND_PACKAGE(Boost COMPONENTS signals REQUIRED)
IF(SORE_WITH_ENET)
	FIND_PACKAGE(ENet)
ENDIF()
FIND_PACKAGE(Freetype REQUIRED)
IF(UNIX)
	FIND_PACKAGE(Fontconfig REQUIRED)  
ENDIF()
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${ENET_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})

IF(NOT SORE_WITH_VBO)
	ADD_DEFINITIONS(-DSORE_NO_VBO)
ENDIF()

INCLUDE_DIRECTORIES(. 3rdparty gui kernel math renderer)
IF(ENET_FOUND)
	INCLUDE_DIRECTORIES(network)
ENDIF()

ADD_SUBDIRECTORY(gui)
ADD_SUBDIRECTORY(kernel)
ADD_SUBDIRECTORY(math)
ADD_SUBDIRECTORY(renderer)
IF(ENET_FOUND AND SORE_WITH_ENET)
	ADD_SUBDIRECTORY(network)
ENDIF()

SET(CMAKE_VERSION_OUT ${CMAKE_CURRENT_SOURCE_DIR}/sore_cmake_version.h)
SET(CMAKE_VERSION_IN  ${CMAKE_CURRENT_SOURCE_DIR}/sore_cmake_version.h.in)

CONFIGURE_FILE(${CMAKE_VERSION_IN} ${CMAKE_VERSION_OUT})

SET(SORE_JSON_SRCS 3rdparty/json/json_reader.cpp 3rdparty/json/json_value.cpp
	3rdparty/json/json_writer.cpp)

IF(UNIX)
	SET(SORE_PLATFORM_SRCS sore_fileio_linux.cpp sore_font_linux.cpp 
		sore_timing_linux.cpp 3rdparty/inotify-cxx.cpp)
	SET(SORE_PLATFORM_HEADERS sore_fileio_linux.h 3rdparty/inotify-cxx.h)
ENDIF()

IF(WIN32)
	SET(SORE_PLATFORM_SRCS sore_fileio_windows.cpp sore_font_windows.cpp 
		sore_timing_windows.cpp)
	SET(SORE_PLATFORM_HEADERS sore_fileio_windows.h)
ENDIF()

set(SORE_SRCS sore_camera.cpp sore_color.cpp
	sore_fileio.cpp sore_font.cpp
	sore_input.cpp sore_interpolater.cpp sore_logger.cpp
	sore_logginginit.cpp sore_profiler.cpp
	sore_resource.cpp sore_scenegraph.cpp sore_scenenode.cpp
	sore_screen.cpp sore_screeninfo.cpp sore_settings.cpp
	sore_text.cpp sore_timing.cpp sore_util.cpp)

set(SORE_HEADERS sore_camera.h sore_color.h
	sore_dll.h sore_defines.h
	sore_exception.h 
	sore_fileio.h sore_font.h
	sore_input.h sore_interpolater.h sore_logger.h
	sore_logginginit.h sore_noncopyable.h sore_profiler.h 
	sore_resource.h
	sore_scenegraph.h sore_scenenode.h sore_screen.h sore_screeninfo.h
	sore_settings.h sore_text.h sore_timing.h sore_util.h) 

#set up source groups to create VS folders
SOURCE_GROUP("Source Files" FILES ${SORE_SRCS})
SOURCE_GROUP("Header Files" FILES ${SORE_HEADERS})

SOURCE_GROUP("Platform Specific\\Source Files" FILES ${SORE_PLATFORM_SRCS})
SOURCE_GROUP("Platform Specific\\Header Files" FILES ${SORE_PLATFORM_HEADERS})

SOURCE_GROUP("GUI\\Source Files" FILES ${SORE_GUI_SRCS})
SOURCE_GROUP("GUI\\Header Files" FILES ${SORE_GUI_HEADERS})

SOURCE_GROUP("Math\\Source Files" FILES ${SORE_MATH_SRCS})
SOURCE_GROUP("Math\\Header Files" FILES ${SORE_MATH_HEADERS})

SOURCE_GROUP("Kernel\\Source Files" FILES ${SORE_KERNEL_SRCS})
SOURCE_GROUP("Kernel\\Header Files" FILES ${SORE_KERNEL_HEADERS})

SOURCE_GROUP("Network\\Source Files" FILES ${SORE_NETWORK_SRCS})
SOURCE_GROUP("Network\\Header Files" FILES ${SORE_NETWORK_HEADERS})

SOURCE_GROUP("Renderer\\Source Files" FILES ${SORE_RENDERER_SRCS})
SOURCE_GROUP("Renderer\\Header Files" FILES ${SORE_RENDERER_HEADERS})

SOURCE_GROUP("3rd Party\\Source Files" FILES ${SORE_JSON_SRCS})

ADD_LIBRARY(sore_lib SHARED
	${SORE_3RDPARTY_HEADERS}
	${SORE_GUI_HEADERS}
	${SORE_GUI_SRCS}
	${SORE_HEADERS}
	${SORE_JSON_SRCS}
	${SORE_KERNEL_HEADERS}
	${SORE_KERNEL_SRCS}
	${SORE_MATH_HEADERS}
	${SORE_MATH_SRCS} 
	${SORE_NETWORK_HEADERS}
	${SORE_NETWORK_SRCS}
	${SORE_PLATFORM_HEADERS}
	${SORE_PLATFORM_SRCS} 
	${SORE_RENDERER_HEADERS}
	${SORE_RENDERER_SRCS}
	${SORE_SRCS}
)

SET_TARGET_PROPERTIES(
sore_lib
PROPERTIES
	DEBUG_POSTFIX _debug
	OUTPUT_NAME sore
	COMPILE_DEFINITIONS_DEBUG DEBUG
)

TARGET_LINK_LIBRARIES(
	sore_lib
	${Boost_LIBRARIES} 
	${FONTCONFIG_LIBRARIES} 
	${FREETYPE_LIBRARY} ${ZLIB_LIBRARY}
	${GLEW_LIBRARY} 
	${OPENGL_gl_LIBRARY} 
	${OPENGL_glu_LIBRARY}
	${SDL_LIBRARY}
)

IF(ENET_FOUND)
	TARGET_LINK_LIBRARIES(sore_lib ${ENET_LIBRARIES})
ENDIF()

INSTALL(TARGETS sore_lib
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

INSTALL(
	FILES 
	${SORE_HEADERS}
	${SORE_PLATFORM_HEADERS}
	${SORE_GUI_HEADERS}
	${SORE_KERNEL_HEADERS}
	${SORE_MATH_HEADERS}
	${SORE_NETWORK_HEADERS}
	${SORE_RENDERER_HEADERS}
	${SORE_3RDPARTY_HEADERS}
	DESTINATION include/sore)
