cmake_minimum_required(VERSION 2.8.8)
project(netsiege)

set(BUILD_TESTS FALSE CACHE BOOL "Build unittests for netsiege components. Requires Qt's QTestLib.")
set(PERMANENTLY_DISABLE_DEBUG_OUTPUT FALSE CACHE BOOL "Permanently disable debug output. Might be useful. Might be not useful.")
set(PROTOBUF_COMPILER "/usr/bin/protoc" CACHE STRING "Google Protocol Buffer compiler, required for building netsiege.")

include(FindPkgConfig REQUIRED)
 

#find_package(OGRE REQUIRED)
#find_package(OIS REQUIRED)

if(BUILD_TESTS)
	find_package(Qt5 REQUIRED Core Test)
	enable_testing()
endif(BUILD_TESTS)

#pkg_search_module(CEGUI REQUIRED CEGUI-0-OGRE)
pkg_search_module(OGRE REQUIRED OGRE)


if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(RELEASE 1)
else()
    set(DEBUG 1)
endif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--std=c++11 -Wall -Wno-deprecated-register" ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})
add_subdirectory(angelscript)
add_subdirectory(oms)
add_subdirectory(network)
add_subdirectory(debug)

add_executable(netsiege main.cpp)
target_link_libraries(netsiege oms network boost_system ndebug Angelscript)

add_executable(netsiege-server server.cpp)
target_link_libraries(netsiege-server oms network boost_system ndebug Angelscript)

install(TARGETS netsiege RUNTIME DESTINATION bin)
