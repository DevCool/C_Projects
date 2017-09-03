# Specify the minimum version for CMake
cmake_minimum_required(VERSION 2.6)
# Project's name
project(SNSH)
# The version number
set (SNSH_VERSION_MAJOR 1)
set (SNSH_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
	)

set (CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set (LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# The following folder will be included
include_directories("${PROJECT_BINARY_DIR}")

# add library
add_library(prs_socket ${PROJECT_SOURCE_DIR}/src/prs_socket/socket.c)

# add the executable
add_executable(SNSH ${PROJECT_SOURCE_DIR}/src/main.c ${PROJECT_SOURCE_DIR}/src/helper.c ${PROJECT_SOURCE_DIR}/src/transfer.c ${PROJECT_SOURCE_DIR}/src/debug.c ${PROJECT_SOURCE_DIR}/src/espeech.c)
target_link_libraries(SNSH prs_socket)
target_link_libraries(SNSH espeak)
add_executable(SNSH_client ${PROJECT_SOURCE_DIR}/src/client/main.c)
target_link_libraries(SNSH_client prs_socket)
add_executable(SNSH_transfer ${PROJECT_SOURCE_DIR}/src/transfer/main.c ${PROJECT_SOURCE_DIR}/src/transfer.c ${PROJECT_SOURCE_DIR}/src/prs_socket/socket.c)
target_link_libraries(SNSH_transfer prs_socket)
