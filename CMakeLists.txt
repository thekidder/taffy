CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(PROJECT_NAME "laffytaffy")
SET(PROJECT_DISPLAY_NAME "laffy taffy")
SET(PROJECT_VERSION_COMMENT "Pre-Alpha")
PROJECT(${PROJECT_NAME})

SET(CPACK_PACKAGE_NAME ${PROJECT_NAME})
SET(CPACK_PACKAGE_VENDOR "kidawi!")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/info.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_CURRENT_SOURCE_DIR}\\\\bin/data/app.ico")
  SET(CPACK_NSIS_DISPLAY_NAME ${PROJECT_DISPLAY_NAME})
  SET(CPACK_NSIS_HELP_LINK "http://www.kidawi.com")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.kidawi.com")
  #SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "${CMAKE_PROJECT_NAME}")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "..\\\\${CMAKE_PROJECT_NAME}" "app.ico")

INCLUDE(CPack)

SET (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

ADD_SUBDIRECTORY(bin)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tools)

INSTALL(FILES boost.txt enet.txt fontconfig.txt freetype.txt glew.txt copyright.txt info.txt lgpl-2.1.txt zlib.txt DESTINATION .)
