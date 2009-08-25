cmake_minimum_required(VERSION 2.6)

FIND_PACKAGE(Boost COMPONENTS program_options filesystem REQUIRED)


SET(CREATE_PACKAGE_SRCS main.cpp walker.h walker.cpp)
ADD_EXECUTABLE(sore_create_package ${CREATE_PACKAGE_SRCS})
TARGET_LINK_LIBRARIES(sore_create_package ${ZLIB_LIBRARY} ${Boost_LIBRARIES})

INSTALL(TARGETS sore_create_package DESTINATION .)
