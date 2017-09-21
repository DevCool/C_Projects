cmake_minimum_required(VERSION 2.6)
project(PSH)
set (PSH_VERSION_MAJOR 1)
set (PSH_VERSION_MINOR 0)
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)

set (CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
#set (LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Follow will be included
include_directories("${PROJECT_BINARY_DIR}")
add_executable(psh ${PROJECT_SOURCE_DIR}/src/main.c ${PROJECT_SOURCE_DIR}/src/psh_cmd.c)
