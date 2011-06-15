CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(Boost_USE_STATIC_LIBS ON)

FIND_PACKAGE(Boost COMPONENTS signals thread date_time REQUIRED)
FIND_PACKAGE(FMODEX REQUIRED)
FIND_PACKAGE(Freetype REQUIRED)
IF(UNIX)
  FIND_PACKAGE(Fontconfig REQUIRED)  
ENDIF(UNIX)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(SFML REQUIRED COMPONENTS Window System)
FIND_PACKAGE(SORE REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${FMODEX_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${SFML_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${LIBSNDFILE_INCLUDE_DIR})

INCLUDE_DIRECTORIES("3rdparty")

ADD_SUBDIRECTORY(3rdparty)

SET(CMAKE_VERSION_OUT ${CMAKE_CURRENT_SOURCE_DIR}/cmake_version.h)
SET(CMAKE_VERSION_IN  ${CMAKE_CURRENT_SOURCE_DIR}/cmake_version.h.in)
CONFIGURE_FILE(${CMAKE_VERSION_IN} ${CMAKE_VERSION_OUT})

SET(${CMAKE_PROJECT_NAME}_SRCS
  app.rc
  averaging_spectrum.cpp
  beat_detector.cpp
  debug_gui.cpp
  energy_analyzer.cpp
  fft_spectrum.cpp
  fmod_pass_through_adapter.cpp
  fmod_spectrum.cpp
  geometric_spectrum.cpp
  graph_visualizer.cpp
  kiss_spectrum.cpp
  main.cpp
  partial_spectrum.cpp
  particle_system.cpp
  sound_pass_through_buffer.cpp
  spectrum.cpp
  spectrum_snapshot.cpp
  spectrum_visualizer.cpp
  state_default.cpp
  utility.cpp
  version_info.cpp)

SET(${CMAKE_PROJECT_NAME}_HEADERS
  app_log.h
  analyzer.h
  averaging_spectrum.h
  beat_detector.h
  bounded_queue.h
  debug_gui.h
  energy_analyzer.h
  fft_spectrum.h
  fmod_pass_through_adapter.h
  fmod_spectrum.h
  geometric_spectrum.h
  graph_visualizer.h
  kiss_spectrum.h
  partial_spectrum.h
  particle_system.h
  sound_pass_through_buffer.h
  spectrum.h
  spectrum_snapshot.h
  spectrum_visualizer.h
  state_default.h
  utility.h
  ${CMAKE_VERSION_OUT})

IF(UNIX)
  SET(CMAKE_CXX_FLAGS "-Wall")
ENDIF(UNIX)

SOURCE_GROUP("Source Files" FILES ${${CMAKE_PROJECT_NAME}_SRCS})
SOURCE_GROUP("Header Files" FILES ${${CMAKE_PROJECT_NAME}_HEADERS})

SOURCE_GROUP("3rd Party\\Source Files" FILES ${KISSFFT_SRCS})
SOURCE_GROUP("3rd Party\\Header Files" FILES ${KISSFFT_HEADERS})


ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} WIN32
  ${${CMAKE_PROJECT_NAME}_SRCS} ${${CMAKE_PROJECT_NAME}_HEADERS} ${KISSFFT_SRCS} ${KISSFFT_HEADERS})

SET_TARGET_PROPERTIES(
  ${CMAKE_PROJECT_NAME}
  PROPERTIES
  DEBUG_POSTFIX _debug
  OUTPUT_NAME ${CMAKE_PROJECT_NAME}
  RUNTIME_OUTPUT_DIRECTORY ../bin
  COMPILE_DEFINITIONS_DEBUG DEBUG
  )
TARGET_LINK_LIBRARIES(
  ${CMAKE_PROJECT_NAME}
  
  ${Boost_LIBRARIES}
  ${FMODEX_LIBRARY}
  ${FONTCONFIG_LIBRARIES}
  ${FREETYPE_LIBRARY}
  ${GLEW_LIBRARY}
  ${OPENGL_gl_LIBRARY}
  ${OPENGL_glu_LIBRARY}
  ${SFML_LIBRARY}
  ${SORE_LIBRARY}
  ${ZLIB_LIBRARY}
  )

SET_PROPERTY(TARGET ${CMAKE_PROJECT_NAME} PROPERTY INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib/)

INSTALL(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION .)
