OPTION(SORE_WITH_VBO "Build with support for VBOs instead of vertex arrays" ON)

IF(WIN32)
	SET(Boost_USE_STATIC_LIBS ON)
ENDIF()

FIND_PACKAGE(Boost COMPONENTS signals REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Freetype REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(ENet)
IF(UNIX)
	FIND_PACKAGE(Fontconfig REQUIRED)  
ENDIF()

INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${ENET_INCLUDE_DIR} ${ZLIB_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR})

IF(UNIX)
	SET(SORE_PLATFORM_SPECIFIC sore_fileio_linux.cpp sore_font_linux.cpp 
		sore_timing_linux.cpp 3rdparty/inotify-cxx.cpp)
	SET(SORE_PLATFORM_HEADERS sore_fileio_linux.h)
	SET(SORE_3RDPARTY_HEADERS 3rdparty/inotify-cxx.h)
ENDIF()

IF(WIN32)
SET(SORE_PLATFORM_SPECIFIC sore_fileio_windows.cpp sore_font_windows.cpp 
	sore_timing_windows.cpp)
ENDIF()

IF(NOT SORE_WITH_VBO)
	ADD_DEFINITIONS(-DSORE_NO_VBO)
ENDIF()

INCLUDE_DIRECTORIES(. gui kernel math)
IF(ENET_FOUND)
	INCLUDE_DIRECTORIES(network)
ENDIF()

ADD_SUBDIRECTORY(gui)
ADD_SUBDIRECTORY(kernel)
ADD_SUBDIRECTORY(math)
IF(ENET_FOUND)
	ADD_SUBDIRECTORY(network)
ENDIF()

SET(CMAKE_VERSION_OUT ${CMAKE_CURRENT_SOURCE_DIR}/sore_cmake_version.h)
SET(CMAKE_VERSION_IN  ${CMAKE_CURRENT_SOURCE_DIR}/sore_cmake_version.h.in)

CONFIGURE_FILE(${CMAKE_VERSION_IN} ${CMAKE_VERSION_OUT})

set(SORE_SRCS sore_batch.cpp sore_color.cpp sore_fbo.cpp
	sore_fileio.cpp sore_font.cpp sore_geometrychunk.cpp
	sore_geometryprovider.cpp sore_graphics.cpp sore_graphicsarray.cpp
	sore_input.cpp sore_interpolater.cpp sore_logger.cpp
	sore_logginginit.cpp sore_profiler.cpp
	sore_renderable.cpp sore_renderer.cpp sore_renderer_std.cpp
	sore_renderstate.cpp
	sore_resource.cpp sore_scenegraph.cpp sore_scenenode.cpp
	sore_screen.cpp sore_screeninfo.cpp sore_settings.cpp
	sore_shaders.cpp sore_sprite.cpp sore_text.cpp
	sore_texture.cpp sore_texturestate.cpp sore_timing.cpp 
	sore_uniformstate.cpp sore_util.cpp
	sore_vbo.cpp sore_vertexarray.cpp)

set(SORE_HEADERS sore_allgl.h sore_batch.h sore_color.h
	sore_dll.h sore_defines.h sore_fbo.h sore_fileio.h sore_font.h
	sore_geometrychunk.h sore_geometryprovider.h sore_graphics.h 
	sore_graphicsarray.h sore_input.h sore_interpolater.h sore_logger.h
	sore_logginginit.h sore_noncopyable.h sore_profiler.h 
	sore_renderable.h
	sore_renderer.h sore_renderer_std.h sore_renderstate.h sore_resource.h
	sore_scenegraph.h sore_scenenode.h sore_screen.h sore_screeninfo.h
	sore_settings.h sore_shaders.h sore_sprite.h sore_text.h
	sore_texture.h sore_texturestate.h sore_timing.h sore_uniformstate.h 
	sore_util.h sore_vbo.h sore_vertexarray.h) 

SOURCE_GROUP("Source Files" FILES ${SORE_SRCS})
SOURCE_GROUP("Header Files" FILES ${SORE_HEADERS})

SOURCE_GROUP("Source Files\\GUI" FILES ${SORE_GUI_SRCS})
SOURCE_GROUP("Header Files\\GUI" FILES ${SORE_GUI_HEADERS})

SOURCE_GROUP("Source Files\\Math" FILES ${SORE_MATH_SRCS})
SOURCE_GROUP("Header Files\\Math" FILES ${SORE_MATH_HEADERS})

SOURCE_GROUP("Source Files\\Kernel" FILES ${SORE_KERNEL_SRCS})
SOURCE_GROUP("Header Files\\Kernel" FILES ${SORE_KERNEL_HEADERS})

SOURCE_GROUP("Source Files\\Network" FILES ${SORE_NETWORK_SRCS})
SOURCE_GROUP("Header Files\\Network" FILES ${SORE_NETWORK_HEADERS})

ADD_LIBRARY(sore_lib SHARED
	${SORE_SRCS} ${SORE_GUI_SRCS} ${SORE_NETWORK_SRCS} ${SORE_MATH_SRCS} 
	${SORE_KERNEL_SRCS} ${SORE_PLATFORM_SPECIFIC} 
	${SORE_HEADERS} ${SORE_GUI_HEADERS} ${SORE_KERNEL_HEADERS}
	${SORE_MATH_HEADERS} ${SORE_NETWORK_HEADERS}
	${SORE_PLATFORM_HEADERS} ${SORE_3RDPARTY_HEADERS})

SET_TARGET_PROPERTIES(
sore_lib
PROPERTIES
	DEBUG_POSTFIX _debug
	OUTPUT_NAME sore
	COMPILE_DEFINITIONS_DEBUG DEBUG
)

TARGET_LINK_LIBRARIES(sore_lib ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY}
${Boost_LIBRARIES} ${FREETYPE_LIBRARY} ${ZLIB_LIBRARY}
${FONTCONFIG_LIBRARIES} ${GLEW_LIBRARY} ${SDL_LIBRARY})

IF(ENET_FOUND)
	TARGET_LINK_LIBRARIES(sore_lib ${ENET_LIBRARIES})
ENDIF()

INSTALL(TARGETS sore_lib
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

INSTALL(FILES ${SORE_HEADERS} DESTINATION include)
INSTALL(FILES ${SORE_NETWORK_HEADERS} DESTINATION include/network)
INSTALL(FILES ${SORE_GUI_HEADERS} DESTINATION include/gui)
INSTALL(FILES ${SORE_MATH_HEADERS} DESTINATION include/math)
INSTALL(FILES ${SORE_PLATFORM_HEADERS} DESTINATION include)
INSTALL(FILES ${SORE_3RDPARTY_HEADERS} DESTINATION include/3rdparty)