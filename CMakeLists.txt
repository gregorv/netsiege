cmake_minimum_required(VERSION 2.8.8)
project(netsiege)


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
# HACK: For some reason, the Ogre_CFLAGS list is strangely handled reagarding the separator,
# leading to strange errors during compilation ("No input files", "File not found", etc).
# To fix this, we replace the semicolons manualy..
string(REPLACE ";" " " Ogre_CFLAGS "${Ogre_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -Wno-deprecated-register -DAS_CAN_USE_CPP11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Boost_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Protobuf_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OIS_CFlAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Ogre_CFLAGS}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(netsiege_COMMON_LIBRARIES
    oms network campaign ndebug script Angelscript
    ${Boost_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
    ${OIS_LIBRARIES}
    ${OGRE_LIBRARIES}
)
set(netsiege_LIBRARIES ${netsiege_COMMON_LIBRARIES} ${OIS_LIBRARIES})
set(netsiege_server_LIBRARIES ${netsiege_COMMON_LIBRARIES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/src
                    ${Boost_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIR} ${OGRE_INCLUDE_DIR} ${OIS_INCLUDE_DIR}
	            )
link_directories(${Boost_LIBRARY_DIR} ${PROTOBUF_LIBRARY_DIR} ${OGRE_LIBRARY_DIR} ${OIS_LIBRARY_DIR})
add_subdirectory(src)

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
