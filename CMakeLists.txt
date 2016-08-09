cmake_minimum_required (VERSION 2.6)

#hush, annoying warnings
if(POLICY CMP0017)
 cmake_policy(SET CMP0017 NEW)
endif(POLICY CMP0017)

project (ASCII-Artinator)

enable_testing()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/modules/")

include(CheckIncludeFileCXX)

# The version number.
set(ASCII-ARTINATOR_VERSION_MAJOR 1)
set(ASCII-ARTINATOR_VERSION_MINOR 0)


set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} --std=gnu++11 -Wall -pthread")

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g2")

check_include_file_cxx(memory HAVE_MEMORY)

if(NOT HAVE_MEMORY)
	message(FATAL_ERROR "Could not find C++11 includes")
endif(NOT HAVE_MEMORY)

find_package(Argtable REQUIRED)

find_package(png++ REQUIRED)

find_package(freetype REQUIRED)

include_directories("${png++_INCLUDE_DIRS};${Argtable_INCLUDES};${FREETYPE_INCLUDE_DIRS}")
 
add_subdirectory ("src")
 
