cmake_minimum_required(VERSION 2.6)
project(netsiege)

set(BUILD_TESTS FALSE CACHE BOOL "Build unittests for netsiege components. Requires Qt's QTestLib.")
set(PROTOBUF_COMPILER "/usr/bin/protoc" CACHE STRING "Google Protocol Buffer compiler, required for building netsiege.")

include(FindPkgConfig REQUIRED)
if(BUILD_TESTS)
	enable_testing()
	find_package(Qt4 REQUIRED QtCore QtTest)
endif(BUILD_TESTS)

if(WIN32)
	set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
	set(OGRE_SAMPLES_INCLUDEPATH
		$ENV{OGRE_HOME}/Samples/include
	)
endif(WIN32)
 
if(UNIX)
	if(EXISTS "/usr/local/lib/OGRE/cmake")
	  set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/local/share/OGRE/samples/Common/include/") # We could just *assume* that developers uses this basepath : /usr/local

	elseif(EXISTS "/usr/lib/OGRE/cmake")

	  set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/share/OGRE/samples/Common/include/") # Otherwise, this one
	elseif(EXISTS "/usr/share/OGRE/cmake")

	  set(CMAKE_MODULE_PATH "/usr/share/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/share/OGRE/samples/Common/include/")

	else ()
	  message(SEND_ERROR "Failed to find module path.")
	endif(EXISTS "/usr/local/lib/OGRE/cmake")
endif(UNIX)

find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)
#pkg_search_module(CEGUI REQUIRED CEGUI-0-OGRE)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--std=c++11 -Wall -Wno-deprecated-register")

include_directories(${CMAKE_SOURCE_DIR})
add_subdirectory(oms)
add_subdirectory(network)

file(WRITE "flags.h" "#ifndef FLAGS_H\n#define FLAGS_H\n")
#export debug/release flag
if (CMAKE_BUILD_TYPE STREQUAL "Debug") 
  message("-- Writing debug flag")
  file(APPEND "flags.h" "#define NDEBUG\n")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
  #message("-- ")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug") 
file(APPEND "flags.h" "#endif")

add_executable(netsiege main.cpp)

# link netsiege components together
target_link_libraries(netsiege oms network)

install(TARGETS netsiege RUNTIME DESTINATION bin)
