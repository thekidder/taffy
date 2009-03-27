cmake_minimum_required(VERSION 2.6)

#set_directory_properties(COMPILE_DEFINITIONS_DEBUG DEBUG);

SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREAD OFF)
FIND_PACKAGE(Boost 1.35 REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(FreeType REQUIRED)
FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
IF(UNIX)
    FIND_PACKAGE(Fontconfig REQUIRED)  
ENDIF(UNIX)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(ENET REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS})

set(INOTIFY inotify-cxx.cpp inotify-cxx.h)

ADD_SUBDIRECTORY("gui")
ADD_SUBDIRECTORY("network")

set(SORE_SRCS
  sore_color.cpp sore_fbo.cpp sore_fileio.cpp sore_font.cpp sore_gamekernel.cpp 
  sore_gamestate.cpp sore_gamestate_manager.cpp
  sore_geometrychunk.cpp sore_graphics.cpp sore_input.cpp sore_interpolater.cpp
  sore_logger.cpp sore_logginginit.cpp
  sore_material.cpp sore_math.cpp sore_particle.cpp sore_particle_circle.cpp sore_particle_emitter.cpp sore_particle_explosion.cpp
  sore_postprocess.cpp sore_profiler.cpp
  sore_renderer.cpp sore_renderer2d.cpp sore_resource.cpp
  sore_scenegraph.cpp sore_scenenode.cpp sore_screen.cpp sore_screenglow.cpp
  sore_screeninfo.cpp sore_settings.cpp sore_shaders.cpp sore_task.cpp
  sore_text.cpp sore_texture.cpp sore_timing.cpp sore_util.cpp 
  sore_vbo.cpp ${SORE_GUI_SRCS} ${SORE_NETWORK_SRCS})

set(SORE_HEADERS sore.h
  sore_allgl.h sore_color.h sore_defines.h sore_fbo.h sore_fileio.h sore_font.h
  sore_gamestate.h sore_gamestate_manager.h
  sore_gamekernel.h sore_geometrychunk.h sore_graphics.h sore_input.h
  sore_interpolater.h sore_logger.h sore_logginginit.h
  sore_material.h sore_math.h math/sore_vector2.h math/sore_vector3.h
  math/sore_vector4.h math/sore_matrix4x4.h sore_particle.h sore_particle_circle.h sore_particle_emitter.h sore_particle_explosion.h
  sore_postprocess.h
  sore_profiler.h sore_renderer.h sore_renderer2d.h sore_resource.h
  sore_scenegraph.h sore_scenenode.h sore_screen.h sore_screenglow.h sore_screeninfo.h
  sore_settings.h sore_shaders.h sore_task.h sore_text.h sore_texture.h
  sore_timing.h sore_util.h sore_vbo.h ${SORE_GUI_HEADERS} ${SORE_NETWORK_HEADERS})

IF(UNIX)
SET(SORE_PLATFORM_SPECIFIC sore_fileio_linux.cpp sore_font_linux.cpp sore_timing_linux.cpp ${INOTIFY})
ENDIF(UNIX)
IF(WIN32)
SET(SORE_PLATFORM_SPECIFIC sore_fileio_windows.cpp sore_font_windows.cpp sore_timing_windows.cpp)
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