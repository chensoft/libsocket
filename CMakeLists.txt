cmake_minimum_required(VERSION 2.8)
project(chen)

# include path
include_directories(./)

# source codes
aux_source_directory(./data SRC_DATA)
aux_source_directory(./lang SRC_LANG)
aux_source_directory(./sync SRC_SYNC)
aux_source_directory(./tool SRC_TOOL)

# platform codes
if (UNIX)
  add_definitions(-DCHEN_OS_UNIX)
  aux_source_directory(./os/unix SRC_OS)
elseif(WIN32)
  add_definitions(-DCHEN_OS_WIN32)
  aux_source_directory(./os/win32 SRC_OS)
else()
  message(FATAL_ERROR "Unsupported system")
endif()

# unit test
if ((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL "") OR (CHEN_BUILD_UNIT STREQUAL "true"))
  message("-- Unit enabled")
  aux_source_directory(./unit SRC_UNIT)
endif()

# generate lib
add_library(chen STATIC ${SRC_DATA} ${SRC_LANG} ${SRC_SYNC} ${SRC_TOOL} ${SRC_OS} ${SRC_UNIT})