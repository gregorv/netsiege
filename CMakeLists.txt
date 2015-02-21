cmake_minimum_required(VERSION 2.8.8)
project(netsiege)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Packages/ ${CMAKE_MODULE_PATH})

set(BUILD_TESTS FALSE CACHE BOOL "Build unittests for netsiege components. Requires Qt's QTestLib.")
set(PERMANENTLY_DISABLE_DEBUG_OUTPUT FALSE CACHE BOOL "Permanently disable debug output. Might be useful. Might be not useful.")
set(PROTOBUF_COMPILER "/usr/bin/protoc" CACHE STRING "Google Protocol Buffer compiler, required for building netsiege.")

 
if(BUILD_TESTS)
	find_package(Qt5 REQUIRED Core Test)
	enable_testing()
endif(BUILD_TESTS)


find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Boost REQUIRED system filesystem)

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(RELEASE 1)
else()
    set(DEBUG 1)
endif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -Wno-deprecated-register -DAS_CAN_USE_CPP11")
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OGRE_INCLUDE_DIRS})
include_directories(${OGRE_Terrain_INCLUDE_DIRS})
include_directories(${OGRE_Paging_INCLUDE_DIRS})
include_directories(${OIS_INCLUDE_DIRS})
include_directories(${PROTOBUF_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
link_directories(${OGRE_LIBRARY_DIRS})
link_directories(${PROTOBUF_INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(netsiege_COMMON_LIBRARIES
    oms network campaign ndebug script Angelscript
    ${Boost_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
    ${OGRE_LIBRARIES}
    ${OGRE_Terrain_LIBRARIES}
    ${OGRE_Paging_LIBRARIES}
)
set(netsiege_LIBRARIES ${netsiege_COMMON_LIBRARIES} ${OIS_LIBRARIES})
set(netsiege_server_LIBRARIES ${netsiege_COMMON_LIBRARIES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/extern/tinyxml/)

add_executable(netsiege src/main.cpp)
target_link_libraries(netsiege ${netsiege_LIBRARIES})

add_executable(netsiege-server src/server.cpp)
target_link_libraries(netsiege-server ${netsiege_server_LIBRARIES})

# link the campaign directory into the build dir,
# so we can just start the damn thing without
# setting environment variables.
add_custom_command(
	TARGET netsiege POST_BUILD
	COMMAND ln -fs ${CMAKE_SOURCE_DIR}/campaigns ${CMAKE_BINARY_DIR}
)


install(TARGETS netsiege RUNTIME DESTINATION bin)
