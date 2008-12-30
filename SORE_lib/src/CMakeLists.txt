cmake_minimum_required(VERSION 2.6)

#set_directory_properties(COMPILE_DEFINITIONS_DEBUG DEBUG);

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Freetype REQUIRED)
FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
IF(UNIX)
    FIND_PACKAGE(Fontconfig REQUIRED)  
ENDIF(UNIX)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(SDL REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS})

set(INOTIFY inotify-cxx.cpp inotify-cxx.h)

set(SORE_SRCS
  sore_fileio.cpp sore_font.cpp sore_geometrychunk.cpp
  sore_graphics.cpp sore_input.cpp sore_interpolater.cpp
  sore_kernel.cpp sore_logger.cpp sore_logginginit.cpp
  sore_material.cpp sore_math.cpp sore_network_client.cpp
  sore_network_server.cpp sore_network.cpp sore_profiler.cpp
  sore_renderer.cpp sore_renderer2d.cpp sore_resource.cpp
  sore_scenegraph.cpp sore_scenenode.cpp sore_screen.cpp
  sore_screeninfo.cpp sore_settings.cpp sore_shaders.cpp
  sore_texture.cpp sore_timing.cpp sore_util.cpp sore_vbo.cpp)

set(SORE_HEADERS sore.h
  sore_allgl.h sore_defines.h sore_fileio.h sore_font.h
  sore_gamestate.h sore_geometrychunk.h
  sore_graphics.h sore_input.h
  sore_interpolater.h sore_kernel.h sore_logger.h sore_logginginit.h
  sore_material.h sore_math.h math/sore_vector2.h math/sore_vector3.h
  math/sore_vector4.h math/sore_matrix4x4.h sore_network.h
  sore_profiler.h sore_renderer.h sore_renderer2d.h sore_resource.h
  sore_scenegraph.h sore_scenenode.h sore_screen.h sore_screeninfo.h
  sore_settings.h sore_shaders.h sore_task.h sore_texture.h
  sore_timing.h sore_util.h sore_vbo.h)

IF(UNIX)
SET(SORE_PLATFORM_SPECIFIC sore_fileio_linux.cpp sore_font_linux.cpp sore_timing_linux.cpp ${INOTIFY})
ENDIF(UNIX)
IF(WIN32)
SET(SORE_PLATFORM_SPECIFIC )
ENDIF(WIN32)

ADD_LIBRARY(sore_lib STATIC ${SORE_SRCS} ${SORE_PLATFORM_SPECIFIC} ${SORE_HEADERS})
SET_TARGET_PROPERTIES(
sore_lib
PROPERTIES
  DEBUG_POSTFIX _d
  OUTPUT_NAME sore
  ARCHIVE_OUTPUT_DIRECTORY ../bin
	COMPILE_DEFINITIONS_DEBUG DEBUG
)
ADD_DEPENDENCIES(sore_lib svnversion_sore)