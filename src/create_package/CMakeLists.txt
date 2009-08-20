cmake_minimum_required(VERSION 2.6)

IF(UNIX)
  SET(CREATE_PACKAGE_SRCS main.cpp walker.h walker_unix.cpp)
  ADD_EXECUTABLE(sore_create_package ${CREATE_PACKAGE_SRCS})
  TARGET_LINK_LIBRARIES(sore_create_package ${ZLIB_LIBRARY})

  INSTALL(TARGETS sore_create_package DESTINATION bin)
ENDIF(UNIX)
