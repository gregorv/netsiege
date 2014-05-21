cmake_minimum_required(VERSION 2.8.8)
project(netsiege)

set(BUILD_TESTS FALSE CACHE BOOL "Build unittests for netsiege components. Requires Qt's QTestLib.")
set(PROTOBUF_COMPILER "/usr/bin/protoc" CACHE STRING "Google Protocol Buffer compiler, required for building netsiege.")

include(FindPkgConfig REQUIRED)
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
find_package(Qt5 REQUIRED Core Test)
if(BUILD_TESTS)
	enable_testing()
endif(BUILD_TESTS)

#pkg_search_module(CEGUI REQUIRED CEGUI-0-OGRE)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--std=c++11 -Wall -Wno-deprecated-register" ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(${CMAKE_SOURCE_DIR} ${Qt5Core_INCLUDES} ${Qt5Test_INCLUDES})
add_subdirectory(oms)
add_subdirectory(network)


add_executable(netsiege main.cpp)
qt5_use_modules(netsiege Core)
target_link_libraries(netsiege oms network boost_system QtCore)

add_executable(netsiege-server server.cpp)
qt5_use_modules(netsiege-server Core)
target_link_libraries(netsiege-server oms network boost_system QtCore)

install(TARGETS netsiege RUNTIME DESTINATION bin)
