CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

ADD_SUBDIRECTORY("create_package")
<<<<<<< HEAD
ADD_SUBDIRECTORY("math")
ADD_SUBDIRECTORY("gui")
IF(ENET_FOUND)
  ADD_SUBDIRECTORY("network")
ENDIF()

SET(CMAKE_VERSION_OUT ${CMAKE_CURRENT_SOURCE_DIR}/sore_cmake_version.h)
SET(CMAKE_VERSION_IN  ${CMAKE_CURRENT_SOURCE_DIR}/sore_cmake_version.h.in)

CONFIGURE_FILE(${CMAKE_VERSION_IN} ${CMAKE_VERSION_OUT})

set(SORE_SRCS sore_batch.cpp sore_color.cpp sore_fbo.cpp
  sore_fileio.cpp sore_font.cpp sore_gamekernel.cpp sore_gamestate.cpp
  sore_gamestate_manager.cpp sore_geometrychunk.cpp
  sore_geometryprovider.cpp sore_graphics.cpp sore_graphicsarray.cpp
  sore_input.cpp sore_interpolater.cpp sore_logger.cpp
  sore_logginginit.cpp sore_math.cpp sore_profiler.cpp
  sore_renderable.cpp sore_renderer.cpp sore_renderer_std.cpp
  sore_resource.cpp sore_scenegraph.cpp sore_scenenode.cpp
  sore_screen.cpp sore_screeninfo.cpp sore_settings.cpp
  sore_shaders.cpp sore_sprite.cpp sore_task.cpp sore_text.cpp
  sore_texture.cpp sore_texturestate.cpp sore_timing.cpp 
  sore_uniformstate.cpp sore_util.cpp
  sore_vbo.cpp sore_vertexarray.cpp ${SORE_GUI_SRCS}
  ${SORE_NETWORK_SRCS} ${SORE_MATH_SRCS})

set(SORE_HEADERS sore.h sore_allgl.h sore_batch.h sore_color.h
  sore_dll.h sore_defines.h sore_fbo.h sore_fileio.h sore_font.h
  sore_gamestate.h sore_gamestate_manager.h sore_gamekernel.h
  sore_geometrychunk.h sore_geometryprovider.h sore_graphics.h
  sore_graphicsarray.h sore_input.h sore_interpolater.h sore_logger.h
  sore_logginginit.h sore_math.h sore_profiler.h sore_renderable.h
  sore_renderer.h sore_renderer_std.h sore_resource.h
  sore_scenegraph.h sore_scenenode.h sore_screen.h sore_screeninfo.h
  sore_settings.h sore_shaders.h sore_sprite.h sore_task.h sore_text.h
  sore_texture.h sore_texturestate.h sore_timing.h sore_uniformstate.h 
  sore_util.h sore_vbo.h sore_vertexarray.h) 

ADD_LIBRARY(sore_lib SHARED
  ${SORE_SRCS} ${SORE_PLATFORM_SPECIFIC} ${SORE_HEADERS}
  ${SORE_GUI_HEADERS} ${SORE_MATH_HEADERS} ${SORE_NETWORK_HEADERS}
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

INSTALL(TARGETS sore_lib
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
INSTALL(FILES ${SORE_HEADERS} DESTINATION include)
INSTALL(FILES ${SORE_NETWORK_HEADERS} DESTINATION include/network)
INSTALL(FILES ${SORE_GUI_HEADERS} DESTINATION include/gui)
INSTALL(FILES ${SORE_MATH_HEADERS} DESTINATION include/math)
INSTALL(FILES ${SORE_PLATFORM_HEADERS} DESTINATION include)
INSTALL(FILES ${SORE_3RDPARTY_HEADERS} DESTINATION include/3rdparty)
=======
ADD_SUBDIRECTORY("lib")
>>>>>>> 4a897561afcee3d2b6ad3754db10c81efd8da075
