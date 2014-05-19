cmake_minimum_required(VERSION 2.6)
project(netsiege)

set(BUILD_TESTS FALSE CACHE BOOL "Build unittests for netsiege components. Requires Qt's QTestLib.")

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

	else ()
	  message(SEND_ERROR "Failed to find module path.")
	endif(EXISTS "/usr/local/lib/OGRE/cmake")
endif(UNIX)

find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)
pkg_search_module(CEGUI REQUIRED CEGUI-0-OGRE)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--std=c++11 -Wall -Wno-deprecated-register")

add_subdirectory(oms)
add_subdirectory(network)

add_executable(netsiege main.cpp)

# link netsiege components together
target_link_libraries(netsiege oms network)

install(TARGETS netsiege RUNTIME DESTINATION bin)
