CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(project_name)

SET (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

ADD_SUBDIRECTORY(bin)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tools)

INSTALL(FILES boost.txt enet.txt fontconfig.txt freetype.txt glew.txt info.txt lgpl-2.1.txt zlib.txt DESTINATION .)
